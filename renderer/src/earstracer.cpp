#include "tracer.h"

/**
 * EARS.h
 *
 * Adapted from Alexander Rath's original implementation 3/5/2023
 *
 * https://graphics.cg.uni-saarland.de/publications/rath-sig2022.html
 * https://github.com/iRath96/ears/blob/master/mitsuba/src/integrators/path/recursive_path.cpp
 * https://github.com/iRath96/ears/blob/master/LICENSE
 */

EARSTracer::LiOutput EARSTracer::Li(EARSTracer::LiInput &input, PathSampleGenerator& sampler) {
    EARSTracer::LiOutput output;

    if (input.depth > maxBounces) {
        output.markAsLeaf(input.depth);
        return output;
    }

    Intersection iinfo;
    IntersectionData idata;
    SurfaceScatterEvent its;

    bool hit = scene->intersect(input.ray, iinfo, idata);
    output.cost += EARS::COST_BSDF;

    if (!hit) {
        output.markAsLeaf(input.depth);
        return output;
    }

    if (idata.primitive->emissive() && input.depth < 1) {
        output.emitted += idata.primitive->evalEmissionDirect(iinfo, idata);
    }

    if (input.depth >= maxBounces) {
        output.markAsLeaf(input.depth);
        return output;
    }

    //const bool bsdfHasSmoothComponent = true;
    //Vec3f albedo = idata.primitive->material->albedo;
    //const int histogramBinIndex = mapOutgoingDirectionToHistogramBin(input.ray.d());
    //const EARS::Octtree::SamplingNode* samplingNode = nullptr;
    //EARS::Octtree::TrainingNode* trainingNode = nullptr;
    //cache.lookup(mapPointToUnitCube(its.data->p), histogramBinIndex, samplingNode, trainingNode);
    const float splittingFactor = 1.0f;

    const int numSamples = 1;
    Vec3f lrSum(0.0f);
    Vec3f lrSumSquares(0.0f);
    float lrSumCosts = 0.0f;

    float roulettePdf = abs(input.weight).max();
    if (roulettePdf == 0.0f)
        return output;
    if (input.depth > 2 && roulettePdf < 0.1f) {
        if (sampler.nextBoolean(DiscreteRouletteSample, roulettePdf))
            input.weight /= roulettePdf;
        else
            return output;
    }

    for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++) {
        Vec3f irradianceEstimate(0.0f);
        Vec3f LrEstimate(0.0f);
        float LrCost(0.0f);

        its = makeLocalScatterEvent(iinfo, idata, input.ray, &sampler);

        /* ==================================================================== */
        /*                     Direct illumination sampling                     */
        /* ==================================================================== */

        LrCost += EARS::COST_NEE;
        LightSample lightsample;
        auto light = scene->primitives.at("Light");

        /* Sample direct illumination */
        Vec3f value(0.0f);
        if (light->sampleLightDirect(its.data->p, *its.sampler, lightsample))
            value = light->evalEmissionDirect(iinfo, idata);
        its.wo = its.frame.toLocal(lightsample.d);

        /* Test visibility */
        Ray shadowRay = input.ray.scatter(its.data->p, lightsample.d, its.data->epsilon);
        Intersection ishadow;
        IntersectionData dshadow;
        if (scene->intersect(shadowRay, ishadow, dshadow) && dshadow.primitive != light.get())
            value *= 0.0f;

        /* Attenuate direct illumination with bsdf */
        Vec3f bsdfVal = its.data->primitive->evalBsdf(its);
        if (bsdfVal != 0.0f) {
            float bsdfPdf = its.data->primitive->bsdfPdf(its);
            float misWeight = powerHeuristic(lightsample.pdf, bsdfPdf);
            float absCosTheta = std::abs(its.wo.z());

            LrEstimate += bsdfVal * value * misWeight / lightsample.pdf;
            irradianceEstimate += absCosTheta * value * misWeight;
        }

        /* ==================================================================== */
        /*                            BSDF sampling                             */
        /* ==================================================================== */

        Vec3f bsdfWeight(0.0f);
        float bsdfPdf;
        Vec3f LiEstimate(0.0f);

        do {
            EARSTracer::LiInput inputNested = input;
            inputNested.weight *= 1.f / splittingFactor;
            Intersection iinfoNested = iinfo;
            IntersectionData idataNested = idata;
            Ray& rayNested = inputNested.ray;
            SurfaceScatterEvent itsNested = makeLocalScatterEvent(iinfoNested, idataNested, rayNested, &sampler);
            if (!itsNested.data->primitive->sampleBsdf(itsNested) || itsNested.weight == 0.0f)
                break;
            bsdfWeight = itsNested.weight;
            bsdfPdf = itsNested.pdf;
            // TODO: Maybe transform to world
            float absCosTheta = std::abs(itsNested.wo.z());

            Vec3f wo = itsNested.frame.toGlobal(itsNested.wo);
            bool hitEmitter = false;
            Vec3f value;

            rayNested = Ray(itsNested.data->p, wo);
            LrCost += EARS::COST_BSDF;
            if (scene->intersect(rayNested, iinfoNested, idataNested)) {
                itsNested = makeLocalScatterEvent(iinfoNested, idataNested, rayNested, &sampler);
                if (idataNested.primitive->emissive()) {
                    value = idataNested.primitive->evalEmissionDirect(iinfoNested, idataNested);
                    hitEmitter = true;
                }
            }
            else {
                break;
            }

            inputNested.weight *= bsdfWeight;
            if (hitEmitter) {
                LightSample lightsampleNested;
                itsNested.data->primitive->sampleLightDirect(itsNested.data->p, sampler, lightsampleNested);
                float lumPdf = lightsampleNested.pdf;
                float misWeight = powerHeuristic(bsdfPdf, lumPdf);
                LrEstimate += bsdfWeight * value * misWeight;
                irradianceEstimate += absCosTheta * (value / bsdfPdf) * misWeight;
            }

            /* ==================================================================== */
            /*                         Indirect illumination                        */
            /* ==================================================================== */
            inputNested.depth++;
            EARSTracer::LiOutput outputNested = Li(inputNested, sampler);
            LrEstimate += bsdfWeight * outputNested.totalContribution();
            irradianceEstimate += absCosTheta * (outputNested.totalContribution() / bsdfPdf);
            LrCost += outputNested.cost;
            output.depthAcc += outputNested.depthAcc;
            output.depthWeight += outputNested.depthWeight;

        } while (false);

        output.reflected += LrEstimate / splittingFactor;
        output.cost += LrCost;
    }
    return output;
}

Vec3f EARSTracer::trace(const Vec2i& px, PathSampleGenerator& sampler) {
    const Vec3f nanDirColor = Vec3f(0.0f);
    const Vec3f nanEnvDirColor = Vec3f(0.0f);
    const Vec3f nanBsdfColor = Vec3f(0.0f);

    Camera cam = scene->camera;
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point, sampler);
    cam.sampleDirection(px, direction, sampler);
    sampler.advancePath();

    Ray ray(point.p, direction.d);
    Vec3f throughput(1.0f);
    int bounce = 0;
    EARSTracer::LiInput input {throughput, ray, bounce, true};

    EARSTracer::LiOutput output = Li(input, sampler);
    return output.totalContribution();
}
