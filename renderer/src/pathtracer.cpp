#include "pathtracer.h"

vec3f DebugPathTracer::trace(const Scene &scene, const vec2i &px) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    float aspect = cam.aspect;
    return glm::abs(vec3f(-1.0f + 2.0f * (float(px.x) / float(resx)),
                 aspect - 2.0f * aspect * (float(px.y) / float(resy)),
                 1));
}
