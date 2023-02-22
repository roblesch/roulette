#include "tracer.h"

Vec3f CameraDebugTracer::trace(const Vec2i& px, PathSampleGenerator &sampler) {
    Camera cam = scene->camera;
    int resx = cam.resx;
    int resy = cam.resy;
    float aspect = cam.aspect;
    return Vec3f(
        -1.0f + 2.0f * (float(px.x()) / float(resx)),
        aspect - 2.0f * aspect * (float(px.y()) / float(resy)),
        1).abs();
}

Vec3f IntersectionDebugTracer::trace(const Vec2i& px, PathSampleGenerator& sampler) {
    Camera cam = scene->camera;
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point, sampler);
    cam.sampleDirection(px, direction, sampler);

    Ray ray(point.p, direction.d);

    Intersection intersection;
    IntersectionData data;
    scene->intersect(ray, intersection, data);

    return data.primitive
        ? (data.Ns + 1.0f).normalized()
        : Vec3f(0.0f);
}

Vec3f AlbedoTracer::trace(const Vec2i& px, PathSampleGenerator& sampler) {
    Camera cam = scene->camera;
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point, sampler);
    cam.sampleDirection(px, direction, sampler);

    Ray ray(point.p, direction.d);

    Intersection intersection;
    IntersectionData data;
    scene->intersect(ray, intersection, data);

    return data.primitive
        ? data.primitive->material->albedo
        : Vec3f(0.0f);
}

Vec3f NormalTracer::trace(const Vec2i& px, PathSampleGenerator &sampler) {
    Camera cam = scene->camera;
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point, sampler);
    cam.sampleDirection(px, direction, sampler);

    Ray ray(point.p, direction.d);

    Intersection intersection;
    IntersectionData data;
    scene->intersect(ray, intersection, data);

    return data.primitive
        ? data.Ns.normalized()
        : Vec3f(0.0f);
}
