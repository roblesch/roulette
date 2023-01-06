#pragma once

#include "usings.h"

class Ray {
public:
    Ray(const vec3f &o, const vec3f &d) :
            o(o),
            d(normalize(d)),
            t(0) {};

    [[nodiscard]] vec3f at(float dt) const { return o + d * dt; }

    vec3f o;
    vec3f d;
    float t;
};
