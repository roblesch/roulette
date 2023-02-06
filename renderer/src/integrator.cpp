#include "integrator.h"

void RayCastIntegrator::render(const Scene &scene, FrameBuffer &frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<IntersectionDebugTracer>(scene);
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            frame.set(px, tracer->trace(px, *sampler));
        }
    }
}

void PathTraceIntegrator::render(const Scene &scene, FrameBuffer &frame) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    tracer = make_unique<PathTracer>(scene);
    for (int j = 0; j < resy; j++) {
        for (int i = 0; i < resx; i++) {
            Vec2i px(i, j);
            Vec3f c(0.0f);
            for (int k = 0; k < 4; k++) {
                sampler->startPath(i + j, 999999999);
                c += tracer->trace(px, *sampler);
            }
            frame.set(px, c/4.0f);
        }
    }
}
