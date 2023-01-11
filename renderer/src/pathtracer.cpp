#include "pathtracer.h"

Vec3f CameraDebugTracer::trace(const Scene &scene, const Vec2i &px) {
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
    Ray ray(cam.eye, cam.sampleDirection(px));
    IntersectionPrimitive intersection;
    scene.intersect(ray, intersection);
    if (intersection.material) {
        return intersection.material->albedo;
    }
    return cam.sampleDirection(px);
}
