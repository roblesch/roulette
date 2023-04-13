/* Adapted from Benedikt Bitterli's Tungsten
 * https://github.com/tunabrain/tungsten */

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "usings.h"

#include "ray.h"
#include "sampler.h"

class Primitive;
class Material;

struct LightSample
{
    Vec3f d;
    float dist;
    float pdf;
};

struct Intersection
{
    const Primitive* primitive;
    Vec3f p;
    bool backface;
    uint8 data[64];

    Intersection() = default;

    template<typename T>
    T* as()
    {
        static_assert(sizeof(T) <= sizeof(data), "Exceeding size of intersection temporary");
        return reinterpret_cast<T*>(&data[0]);
    }
    template<typename T>
    const T* as() const
    {
        static_assert(sizeof(T) <= sizeof(data), "Exceeding size of intersection temporary");
        return reinterpret_cast<const T*>(&data[0]);
    }
};

struct IntersectionData
{
    Vec3f Ng;
    Vec3f Ns;
    Vec3f p;
    Vec3f w;
    Vec2f uv;
    float epsilon;
    bool backSide;

    const Primitive* primitive;
};

struct SurfaceScatterEvent
{
    const IntersectionData* data;
    PathSampleGenerator* sampler;
    TangentFrame frame;
    Vec3f wi, wo;
    Vec3f weight;
    float pdf;
    bool flippedFrame;

    SurfaceScatterEvent() = default;

    SurfaceScatterEvent(IntersectionData& intersection, Ray& ray) {
        TangentFrame frame(intersection.Ns);
        bool hitBackside = frame.normal.dot(ray.d()) > 0.0f;
        if (hitBackside) {
            frame.normal = -frame.normal;
            frame.tangent = -frame.tangent;
        }
        this->data = &intersection;
        this->frame = frame;
        this->wi = frame.toLocal(-ray.d());
        wo = Vec3f(0.0f);
        weight = Vec3f(1.0f);
        pdf = 1.0f;
        flippedFrame = hitBackside;
    }

    SurfaceScatterEvent(const IntersectionData* data_,
        PathSampleGenerator* sampler_,
        const TangentFrame& frame_,
        const Vec3f& wi_,
        bool flippedFrame_) :
        data(data_),
        sampler(sampler_),
        frame(frame_),
        wi(wi_),
        wo(0.0f),
        weight(1.0f),
        pdf(1.0f),
        flippedFrame(flippedFrame_) {};

    SurfaceScatterEvent makeForwardEvent() const
    {
        SurfaceScatterEvent copy(*this);
        copy.wo = -copy.wi;
        return copy;
    }
};

#endif
