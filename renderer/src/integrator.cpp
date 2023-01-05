#include "integrator.h"

#include "camera.h"
#include "framebuffer.h"
#include "scene.h"

void DebugIntegrator::render(Scene& scene, FrameBuffer& frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            vec2i px(i, j);
            vec3f colorizedDirection = (cam.sampleDirection(px) + 1.0f) / 2.0f;
            frame.set(px, colorizedDirection);
        }
    }
};

void PathIntegrator::render(Scene& scene, FrameBuffer& frame) {};
