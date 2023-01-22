#ifndef SAMPLE_H
#define SAMPLE_H

#include "usings.h"

struct PositionSample {
    Vec3f p;
    Vec3f weight;
    float pdf;
    Vec3f Ng;
};

struct DirectionSample {
    Vec3f d;
    Vec3f weight;
    float pdf;
};

#endif
