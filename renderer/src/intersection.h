#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "usings.h"

#include "ray.h"

class Primitive;
class Material;

struct IntersectionData {
    Primitive *primitive = nullptr;
    shared_ptr<Material> material;
    bool front;
    float tnear = F_NEAR_ZERO;
    float tfar = F_INFTY;
    Vec3f normal;
    Vec3f p;
    Vec3f w;
    Vec3f Ns;
    Vec3f Ng;
};

struct SurfaceScatterEvent
{
    const IntersectionData* data;
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

    SurfaceScatterEvent(const IntersectionData* intersection,
        const TangentFrame& frame,
        const Vec3f& wi,
        bool flippedFrame) :
        data(intersection),
        frame(frame),
        wi(wi),
        wo(0.0f),
        weight(1.0f),
        pdf(1.0f),
        flippedFrame(flippedFrame) {};
};

#endif
