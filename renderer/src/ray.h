#ifndef RAY_H
#define RAY_H

#include "usings.h"

class Ray {
public:
    Ray() :
        o(Vec3f(0.0f, 0.0f, 0.0f)),
        d(Vec3f(0.0f, 0.0f, -1.0f)),
        t(0) {};
    Ray(const Vec3f &o, const Vec3f &d) :
            o(o),
            d(d),
            t(0) {};

    [[nodiscard]] Vec3f at(float dt) const { return o + d * dt; }

    Vec3f o;
    Vec3f d;
    float t;
};

#endif
