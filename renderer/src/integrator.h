#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "usings.h"

#include "framebuffer.h"
#include "tracer.h"
#include "sampler.h"
#include "scene.h"

class Integrator {
public:
    virtual ~Integrator() = default;
    virtual void render(const Scene &scene, FrameBuffer &frame) = 0;
    unique_ptr<Tracer> tracer;
    unique_ptr<PathSampleGenerator> sampler;
};

class RayCastIntegrator : public Integrator {
public:
    void render(const Scene &scene, FrameBuffer &frame) override;
};

class PathTraceIntegrator : public Integrator {
public:
    PathTraceIntegrator() {
        sampler = std::unique_ptr<PathSampleGenerator>(new UniformPathSampler(0xBA5EBA11));
    }
    void render(const Scene &scene, FrameBuffer &frame) override;
};

class OIDNIntegrator : public Integrator {
public:
    OIDNIntegrator() {
        sampler = std::unique_ptr<PathSampleGenerator>(new UniformPathSampler(0xBA5EBA11));
    }
    void render(const Scene& scene, FrameBuffer& frame) override;
};

class EARSIntegrator : public Integrator {
public:
    EARSIntegrator() {
        sampler = std::unique_ptr<PathSampleGenerator>(new UniformPathSampler(0xBA5EBA11));
    }
    void render(const Scene& scene, FrameBuffer& frame) override;
};

#endif
