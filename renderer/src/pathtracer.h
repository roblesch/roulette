#ifndef PATHTRACER_H
#define PATHTRACER_H

#include "usings.h"

#include "ray.h"
#include "scene.h"

class Tracer {
public:
    virtual ~Tracer() = default;

    virtual Vec3f trace(const Scene &scene, const Vec2i &px) = 0;
};

class CameraDebugTracer : public Tracer {
public:
    Vec3f trace(const Scene &scene, const Vec2i &px) override;
};

class IntersectionDebugTracer : public Tracer {
public:
    Vec3f trace(const Scene& scene, const Vec2i& px) override;
};

#endif
