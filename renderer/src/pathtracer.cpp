#include "pathtracer.h"

vec3f DebugPathTracer::trace(const Scene &scene, const vec2i &px) {
    Camera cam = scene.camera;
    vec3f d = cam.sampleDirection(px);
    return (d + 1.0f) / 2.0f;
}
