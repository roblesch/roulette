#ifndef RAY_H
#define RAY_H

#include "usings.h"

class Ray {
public:
    Ray() :
        p(Vec3f(0.0f, 0.0f, 0.0f)),
        d(Vec3f(0.0f, 0.0f, -1.0f)),
        t(0) {};
    Ray(const Vec3f &o, const Vec3f &d) :
            p(o),
            d(d),
            t(0) {};

    [[nodiscard]] Vec3f at(float dt) const {
        return p + d * dt;
    }

    const Vec3f& pos() const {
        return p;
    }

    const Vec3f& dir() const {
        return d;
    }

    Vec3f p;
    Vec3f d;
    float t;
};

#endif
