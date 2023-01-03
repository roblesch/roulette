#pragma once

#include "usings.h"

class Scene;
class FrameBuffer;

class Integrator {
public:
    virtual ~Integrator() = default;
    virtual void render(Scene& scene, FrameBuffer& frame) = 0;
};

class DebugIntegrator : public Integrator {
public:
    void render(Scene& scene, FrameBuffer& frame);
};

class PathIntegrator : public Integrator {
public:
    void render(Scene& scene, FrameBuffer& frame);
};
