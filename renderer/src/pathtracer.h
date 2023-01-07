#pragma once

#include "usings.h"

#include "ray.h"
#include "scene.h"

class Tracer {
public:
    virtual ~Tracer() = default;

    virtual vec3f trace(const Scene &scene, const vec2i &px) = 0;
};

class DebugPathTracer : public Tracer {
public:
    vec3f trace(const Scene &scene, const vec2i &px) override;
};
