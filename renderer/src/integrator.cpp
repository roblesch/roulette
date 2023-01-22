#include "integrator.h"

void RayCastIntegrator::render(Scene &scene, FrameBuffer &frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<IntersectionDebugTracer>();
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            frame.set(px, tracer->trace(scene, px));
        }
    }
}

void PathTraceIntegrator::render(Scene &scene, FrameBuffer &frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<PathTracer>();
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            frame.set(px, tracer->trace(scene, px));
        }
    }
}
