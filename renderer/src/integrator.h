#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "usings.h"

#include "framebuffer.h"
#include "pathtracer.h"
#include "scene.h"

class Integrator {
public:
    virtual ~Integrator() = default;

    virtual void render(Scene &scene, FrameBuffer &frame) = 0;

    unique_ptr<Tracer> tracer;
};

class RayCastIntegrator : public Integrator {
public:
    void render(Scene &scene, FrameBuffer &frame) override;
};

class PathTraceIntegrator : public Integrator {
public:
    void render(Scene &scene, FrameBuffer &frame) override;
};

#endif
