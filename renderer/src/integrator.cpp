#include "integrator.h"

void PathIntegrator::render(Scene &scene, FrameBuffer &frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<DebugPathTracer>();
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            vec2i px(i, j);
            frame.set(px, tracer->trace(scene, px));
        }
    }
};
