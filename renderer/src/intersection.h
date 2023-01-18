#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "usings.h"

class Primitive;

class Material;

struct IntersectionPrimitive {
    Primitive *primitive;
    shared_ptr<Material> material;
    bool front;
    float tnear = F_NEAR_ZERO;
    float tfar = F_INFTY;
    Vec3f normal;
    Vec3f p;
};

#endif
