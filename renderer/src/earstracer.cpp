#include "tracer.h"
#include "octtree.h"

/// the cost of ray tracing + direct illumination sample (in seconds)
static constexpr float COST_NEE = 0.3e-7;
/// the cost of ray tracing + BSDF/camera sample (in seconds)
static constexpr float COST_BSDF = 0.3e-7;

inline int mapOutgoingDirectionToHistogramBin(const Vec3f &d) {
    return -1;
}

LiOutput EARSTracer::Li(LiInput &input, PathSampleGenerator& sampler) {
    LiOutput output;

    if (input.depth > maxBounces) {
        output.markAsLeaf(input.depth);
        return output;
    }

    Intersection intersection;
    IntersectionData data;
    SurfaceScatterEvent event;

    bool its = scene->intersect(input.ray, intersection, data);
    output.cost += COST_BSDF;

    if (!its) {
        output.markAsLeaf(input.depth);
    }

    if (data.primitive->emissive()) {
        output.emitted += data.primitive->evalEmissionDirect(intersection, data);
    }

    // TODO: Splitting Factor
    const int numSamples = 1;
    for (int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++) {
        Vec3f irradianceEstimate(0.0f);
        Vec3f LrEstimate(0.0f);
        float LrCost(0.0f);

        event = makeLocalScatterEvent(intersection, data, input.ray, &sampler);

        /* Direct Illumination */
        LrCost += COST_NEE;
        const Primitive* light = scene->primitives.at("Light")->get();
        LrEstimate += lightSample(*light, event, input.depth, input.ray);
        //if (light->sampleLightDirect(event.data->p, *event.sampler, sample)) {
        //    Vec3f bsdfVal = event.data->primitive->evalBsdf(event);
        //    if (bsdfVal != 0.0f) {
        //        float bsdfPdf = event.data->primitive->bsdfPdf(event);
        //        float misWeight = powerHeuristic(sample.pdf, bsdfPdf);
        //        LrEstimate += bsdfVal * misWeight;
        //        irradianceEstimate += bsdfVal * misWeight;
        //    }
        //}

        /* BSDF Sampling */
        //Vec3f bsdfWeight(0.0f);
        //float bsdfPdf;
        //Vec3f LiEstimate(0.0f);

        output.reflected += LrEstimate;
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
    LiInput input {throughput, ray, bounce};

    LiOutput output = Li(input, sampler);
    return output.totalContribution();
}
