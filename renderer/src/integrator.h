#pragma once

#include "usings.h"
#include "scene.h"

class Scene;
class FrameBuffer;

class Integrator {
public:
    Integrator() {};
    void render(Scene& scene, FrameBuffer& frame);
};

class DebugIntegrator : public Integrator {
public:
    DebugIntegrator() {};
    void render(Scene& scene, FrameBuffer& frame);
};

class PathIntegrator : public Integrator {
public:
    PathIntegrator() {};
    void render(Scene& scene, FrameBuffer& frame);
};
