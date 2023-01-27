#include "tracer.h"

bool PathTracer::handleSurface(SurfaceScatterEvent& event, Intersection& intersection, IntersectionData& data,
    int bounce, Ray& ray, Vec3f& throughput, Vec3f& emission) {
    // bsdf = data->primitive->material;

    Vec3f wo;
    if (bounce < maxBounces - 1)
        emission += estimateDirect(event, bounce + 1, ray) * throughput;

    if (data.primitive->emissive() && bounce >= 0) {
        if (!enableLightSampling || wasSpecular || !info.primitive->isSamplable())
            emission += info.primitive->evalDirect(data, info) * throughput;
    }

    event.requestedLobe = BsdfLobes::AllLobes;
    if (!bsdf.sample(event, adjoint))
        return false;

    wo = event.frame.toGlobal(event.wo);

    if (!isConsistent(event, wo))
        return false;

    throughput *= event.weight;
    wasSpecular = event.sampledLobe.hasSpecular();
    if (!wasSpecular)
        ray.setPrimaryRay(false);
    return false;
}

Vec3f PathTracer::trace(const Scene& input, const Vec2i& px) {
    this->scene = &input;
    Camera cam = scene->camera;
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point);
    cam.sampleDirection(px, direction);

    Ray ray(point.p, direction.d);

    Intersection intersection;
    IntersectionData data;
    SurfaceScatterEvent surfaceEvent;
    Vec3f emission(0.0f);
    Vec3f throughput(1.0f);
    int bounce = 0;

    bool hit = scene->intersect(ray, intersection, data);
    while (hit && bounce < maxBounces) {
        surfaceEvent = makeLocalScatterEvent(intersection, data, ray);
        if (!handleSurface(surfaceEvent, intersection, data, bounce, ray, throughput, emission))
            break;
    }

    if (data.primitive) {
        if (data.primitive->emissive()) return Vec3f(1.0);
        else return (data.Ns + 1.0f).normalized();
    }

    return Vec3f(0.0f);
}

/* === */
//int maxBounces = 64;
//
//bool isConsistent(const SurfaceScatterEvent& event, const Vec3f& w) {
//    bool geometricBackside = (w.dot(event.data->Ng) < 0.0f);
//    bool shadingBackside = (event.wo.z() < 0.0f) ^ event.flippedFrame;
//    return geometricBackside == shadingBackside;
//}
//
//Vec3f generalizedShadowRay(const Scene& scene, Ray &ray, const shared_ptr<Primitive> light, int bounce) {
//    IntersectionData data;
//    float tfar_i = ray.tfar();
//    Vec3f throughput(1.0f);
//    do {
//        bool hit = scene.intersect(ray, data) && data.primitive != &(*light);
//        if (hit) {
//            return Vec3f(0.0f);
//        }
//        if (data.primitive == nullptr || data.primitive == &(*light))
//            return throughput;
//        tfar_i -= ray.tfar();
//        ray = Ray(ray.p() + ray.d() * ray.tfar(), ray.d());
//        ray.tfar(tfar_i);
//    } while(true);
//    return Vec3f(1.0f);
//}
//
//Vec3f attenuatedEmission(const Scene& scene, const shared_ptr<Primitive> light, float expectedDist, IntersectionData& data, int bounce, Ray& ray) {
//    float fudgeFactor = 1.0f + 1e-3f;
//    /*ray.tfar(expectedDist);*/
//    if (!light->intersect(ray, data) || ray.tfar() * fudgeFactor < expectedDist)
//        return Vec3f(0.0f);
//    data.p = ray.p() + ray.d() * ray.tfar();
//    data.w = ray.d();
//    light->setIntersectionData(data);
//
//    Vec3f transmittance = generalizedShadowRay(scene, ray, light, bounce); //TODO:: shadowray
//    if (transmittance == 0.0f)
//        return Vec3f(0.0f);
//
//    return transmittance * light->evalDirect(data);
//}
//
//Vec3f lightSample(const Scene& scene, const shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
//    LightSample sample;
//    if (!light->sampleDirect(event.data->p, sample))
//        return Vec3f(0.0f);
//
//    event.wo = event.frame.toLocal(sample.d);
//    if (!isConsistent(event, sample.d))
//        return Vec3f(0.0f);
//
//    Vec3f f = event.data->material->eval(event);
//    if (f == 0.0f)
//        return Vec3f(0.0f);
//
//    Ray ray = parentRay.scatter(event.data->p, sample.d);
//    ray.setPrimary(false);
//
//    IntersectionData data;
//    Vec3f e = attenuatedEmission(scene, light, sample.dist, data, bounce, ray);
//    if (e == 0.0f)
//        return Vec3f(0.0f);
//
//    Vec3f lightF = f * e / sample.pdf;
//    lightF *= powerHeuristic(sample.pdf, event.data->material->pdf(event));
//
//    return lightF;
//}
//
//Vec3f bsdfSample(const Scene& scene, const shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
//    if (!event.data->material->sample(event))
//        return Vec3f(0.0f);
//    if (event.weight == 0.0f)
//        return Vec3f(0.0f);
//
//    Vec3f wo = event.frame.toGlobal(event.wo);
//    if (!isConsistent(event, wo))
//        return Vec3f(0.0f);
//
//    Ray ray = parentRay.scatter(event.data->p, wo);
//    ray.setPrimary(false);
//
//    IntersectionData data;
//    Vec3f e = attenuatedEmission(scene, light, -1.0f, data, bounce, ray);
//
//    if (e == Vec3f(0.0f))
//        return Vec3f(0.0f);
//
//    Vec3f bsdfF = e * event.weight;
//    bsdfF *= powerHeuristic(event.pdf, light->directPdf(data, event.data->p));
//    return bsdfF;
//}
//
//Vec3f sampleDirect(const Scene &scene, const shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
//    Vec3f result(0.0f);
//    result += lightSample(scene, light, event, bounce, parentRay);
//    result += bsdfSample(scene, light, event, bounce, parentRay);
//    return result;
//}
//
//Vec3f estimateDirect(const Scene &scene, SurfaceScatterEvent& event, int bounce, const Ray &parentRay) {
//    float weight = 1.0f;
//    const shared_ptr<Primitive> light = scene.lights.at("Light");
//    return sampleDirect(scene, light, event, bounce, parentRay) * weight;
//}
//
//bool handleSurface(const Scene& scene, SurfaceScatterEvent& event, IntersectionData& data, int bounce, Ray& ray, Vec3f& throughput, Vec3f& emission) {
//    Vec3f wo;
//    const shared_ptr<Material> bsdf = data.material;
//
//    if (bounce < maxBounces - 1)
//        emission += estimateDirect(scene, event, bounce + 1, ray) * throughput;
//    if (data.primitive->emissive())
//        emission += data.primitive->evalDirect(data) * throughput;
//
//    if (!bsdf->sample(event))
//        return false;
//
//    wo = event.frame.toGlobal(event.wo);
//
//    if (!isConsistent(event, wo))
//        return false;
//
//    throughput *= event.weight;
//    ray.setPrimary(false);
//
//    ray = ray.scatter(ray.p() + ray.d() * ray.tfar(), wo);
//    
//    return true;
//}
//
//Vec3f PathTracer::trace(const Scene& scene, const Vec2i& px) {
//    PositionSample point;
//    DirectionSample direction;
//    scene.camera.samplePosition(point);
//    scene.camera.sampleDirection(px, direction);
//
//    Vec3f throughput = point.weight * direction.weight;
//    Ray ray(point.p, direction.d);
//    ray.setPrimary(true);
//
//    SurfaceScatterEvent surfaceEvent;
//    IntersectionData data;
//    Vec3f emission(0.0f);
//
//    int bounce = 0;
//    bool hit = scene.intersect(ray, data);
//    while (hit && bounce < maxBounces) {
//        surfaceEvent = SurfaceScatterEvent(data, ray);
//        if (!handleSurface(scene, surfaceEvent, data, bounce, ray, throughput, emission))
//            break;
//
//        if (throughput.max() == 0.0f)
//            break;
//        
//        float roulettePdf = abs(throughput).max();
//        if (bounce > 2 && roulettePdf < 0.1f) {
//            if (randf() < roulettePdf)
//                throughput /= roulettePdf;
//            else
//                break;
//        }
//
//        if (std::isnan(ray.d().sum() + ray.p().sum()))
//            return Vec3f(0.0f);
//        if (std::isnan(throughput.sum() + emission.sum()))
//            return Vec3f(0.0f);
//
//        bounce++;
//        if (bounce < maxBounces)
//            hit = scene.intersect(ray, data);
//    }
//    if (std::isnan(throughput.sum() + emission.sum()))
//        return Vec3f(0.0f);
//    return emission;
//}
