#include "tracer.h"

Vec3f CameraDebugTracer::trace(const Scene& scene, const Vec2i& px) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    float aspect = cam.aspect;
    return Vec3f(
        -1.0f + 2.0f * (float(px.x()) / float(resx)),
        aspect - 2.0f * aspect * (float(px.y()) / float(resy)),
        1).abs();
}

Vec3f IntersectionDebugTracer::trace(const Scene& scene, const Vec2i& px) {
    Camera cam = scene.camera;
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point);
    cam.sampleDirection(px, direction);

    Ray ray(point.p, direction.d);

    Intersection intersection;
    IntersectionData data;
    scene.intersect(ray, intersection, data);
    if (data.primitive) {
        if (data.primitive->emissive()) return Vec3f(1.0);
        else return (data.Ns + 1.0f).normalized();
    }
    return Vec3f(0.0f);
}