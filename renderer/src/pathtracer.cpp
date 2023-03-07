#include "tracer.h"

inline Vec3f PathTracer::generalizedShadowRayImpl(Ray& ray,
    const Primitive* endCap,
    int bounce,
    bool startsOnSurface,
    bool endsOnSurface,
    float& pdfForward,
    float& pdfBackward) const
{
    bool ComputePdfs = false;
    Intersection intersection;
    IntersectionData data;

    float initialFarT = ray.tfar();
    Vec3f throughput(1.0f);
    do {
        bool didHit = scene->intersect(ray, intersection, data) && data.primitive != endCap;
        if (didHit) {
            SurfaceScatterEvent event = makeLocalScatterEvent(intersection, data, ray, nullptr);

            Vec3f transparency = data.primitive->evalBsdf(event.makeForwardEvent());
            if (transparency == 0.0f)
                return Vec3f(0.0f);

            //if (ComputePdfs) {
            //    float transparencyScalar = transparency.avg();
            //    pdfForward *= transparencyScalar;
            //    pdfBackward *= transparencyScalar;
            //}

            throughput *= transparency;
            bounce++;

            if (bounce >= maxBounces)
                return Vec3f(0.0f);
        }

        if (data.primitive == nullptr || data.primitive == endCap)
            return bounce >= 0 ? throughput : Vec3f(0.0f);
        ray.setP(ray.tip());
        initialFarT -= ray.tfar();
        ray.tnear(data.epsilon);
        ray.tfar(initialFarT);
    } while (true);
    return Vec3f(0.0f);
}

Vec3f PathTracer::generalizedShadowRay(Ray& ray, const Primitive* endCap, int bounce) const {
    float dummyA, dummyB;
    return generalizedShadowRayImpl(ray, endCap, bounce, false, false, dummyA, dummyB);
}

Vec3f PathTracer::attenuatedEmission(const Primitive& light, float expectedDist, Intersection& intersection,
    IntersectionData& data, int bounce, Ray& ray) {
    float fudgeFactor = 1.0f + 1e-3f;

    if (!light.intersect(ray, intersection) || ray.tfar() * fudgeFactor < expectedDist || intersection.backface)
        return Vec3f(0.0f);
    //return Vec3f(1.0f);
    data.p = ray.p() + ray.d() * ray.tfar();
    data.w = ray.d();
    light.setIntersectionData(intersection, data);

    Vec3f transmittance = generalizedShadowRay(ray, &light, bounce);
    if (transmittance == 0.0f)
        return Vec3f(0.0f);

    return transmittance * light.evalEmissionDirect(intersection, data);
}

Vec3f PathTracer::bsdfSample(const Primitive& light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
    if (!event.data->primitive->sampleBsdf(event))
        return Vec3f(0.0f);
    if (event.weight == 0.0f)
        return Vec3f(0.0f);

    Vec3f wo = event.frame.toGlobal(event.wo);
    //if (!isConsistent(event, wo))
    //    return Vec3f(0.0f);

    Ray ray = parentRay.scatter(event.data->p, wo, event.data->epsilon);
    ray.setPrimary(false);

    Intersection intersection;
    IntersectionData data;

    Vec3f e = attenuatedEmission(light, -1.0f, intersection, data, bounce, ray);

    if (e == Vec3f(0.0f))
        return Vec3f(0.0f);

    Vec3f bsdfF = e * event.weight;

    bsdfF *= powerHeuristic(event.pdf, light.shapePdf(intersection, data, event.data->p));

    return bsdfF;
}

Vec3f PathTracer::lightSample(const Primitive& light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
    LightSample sample;
    if (!light.sampleLightDirect(event.data->p, *event.sampler, sample))
        return Vec3f(0.0f);

    event.wo = event.frame.toLocal(sample.d);
    //if (!isConsistent(event, sample.d))
    //    return Vec3f(0.0f);

    Vec3f f = event.data->primitive->evalBsdf(event);
    if (f == 0.0f)
        return Vec3f(0.0f);

    Ray ray = parentRay.scatter(event.data->p, sample.d, event.data->epsilon);
    ray.setPrimary(false);

    Intersection intersection;
    IntersectionData data;

    Vec3f e = attenuatedEmission(light, sample.dist, intersection, data, bounce, ray);
    if (e == 0.0f)
        return Vec3f(0.0f);

    Vec3f lightF = f * e / sample.pdf;
    lightF *= powerHeuristic(sample.pdf, event.data->primitive->bsdfPdf(event));

    return lightF;
}

Vec3f PathTracer::sampleDirect(const Primitive& light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
    Vec3f result(0.0f);
    result += lightSample(light, event, bounce, parentRay);
    event.sampler->advancePath();
    result += bsdfSample(light, event, bounce, parentRay);
    event.sampler->advancePath();
    return result;
}

Vec3f PathTracer::estimateDirect(SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
    float weight = 1.0f;
    const Primitive* light = scene->primitives.at("Light")->get();
    return sampleDirect(*light, event, bounce, parentRay) * weight;
    return Vec3f(0.0f);
}

bool PathTracer::handleSurface(SurfaceScatterEvent& event, Intersection& intersection, IntersectionData& data,
    int bounce, Ray& ray, Vec3f& throughput, Vec3f& emission, bool &wasSpecular) {

    Vec3f wo;
    if (bounce < maxBounces - 1)
        emission += estimateDirect(event, bounce + 1, ray) * throughput;

    if (data.primitive->emissive() && wasSpecular) {
        emission += data.primitive->evalEmissionDirect(intersection, data) * throughput;
    }

    if (!data.primitive->sampleBsdf(event))
        return false;

    wo = event.frame.toGlobal(event.wo);

    //if (!isConsistent(event, wo)) TODO:consistency?
    //    return false;

    throughput *= event.weight;
    if (!event.data->primitive->emissive())
        wasSpecular = false;
    ray.setPrimary(false);
    ray = ray.scatter(ray.p() + ray.d() * ray.tfar(), wo, data.epsilon);

    return true;
}

Vec3f PathTracer::trace(const Vec2i& px, PathSampleGenerator &sampler) {
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

    Intersection intersection;
    IntersectionData data;
    SurfaceScatterEvent surfaceEvent;
    Vec3f emission(0.0f);
    Vec3f throughput(1.0f);
    bool wasSpecular = true;
    int bounce = 0;

    bool hit = scene->intersect(ray, intersection, data);
    while (hit && bounce < maxBounces) {
        surfaceEvent = makeLocalScatterEvent(intersection, data, ray, &sampler);
        if (!handleSurface(surfaceEvent, intersection, data, bounce, ray, throughput, emission, wasSpecular))
            break;

        if (throughput.max() == 0.0f)
            break;

        float roulettePdf = abs(throughput).max();
        if (bounce > 2 && roulettePdf < 0.1f) {
            if (sampler.nextBoolean(DiscreteRouletteSample, roulettePdf))
                throughput /= roulettePdf;
            else
                break;
        }
        if (std::isnan(ray.d().sum() + ray.p().sum()))
            return nanDirColor;
        if (std::isnan(throughput.sum() + emission.sum()))
            return nanBsdfColor;

        sampler.advancePath();
        bounce++;
        if (bounce < maxBounces)
            hit = scene->intersect(ray, intersection, data);
    }
    if (std::isnan(throughput.sum() + emission.sum()))
        return nanEnvDirColor;
    return emission;
}
