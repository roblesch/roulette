#ifndef SAMPLE_H
#define SAMPLE_H

#include "usings.h"

struct TangentFrame {
    explicit TangentFrame(const Vec3f& n) {
        normal = n;
        Vec3f tan = abs(n.x()) > abs(n.y()) ?
            Vec3f(0.0f, 1.0f, 0.0f) :
            Vec3f(1.0f, 0.0f, 0.0f);
        bitangent = n.cross(tan).normalized();
        tangent = bitangent.cross(n);
    }

    Vec3f normal{};
    Vec3f tangent{};
    Vec3f bitangent{};
};

struct PositionSample {
    Vec3f pos;
    Vec3f weight;
    float pdf;
    Vec3f Ng;
};

struct DirectionSample {
    Vec3f dir;
    Vec3f weight;
    float pdf;
};

#endif
