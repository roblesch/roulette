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

class PathTracer : public Tracer {
public:
    Vec3f trace(const Scene& scene, const Vec2i& px) override;
    bool handleSurface(SurfaceScatterEvent& event, Intersection& intersection, IntersectionData& data,
        int bounce, Ray& ray, Vec3f& throughput, Vec3f& emission);
    SurfaceScatterEvent makeLocalScatterEvent(Intersection& intersection, IntersectionData& data, Ray& ray) const {
        TangentFrame frame(data.Ns);

        bool hitBackside = frame.normal.dot(ray.d()) > 0.0f;

        if (hitBackside) {
            frame.normal = -frame.normal;
            frame.tangent = -frame.tangent;
        }

        return SurfaceScatterEvent(
            &data,
            frame,
            frame.toLocal(-ray.d()),
            hitBackside 
        );
    }

    const Scene* scene;
    int maxBounces = 64;
};

#endif
