#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "usings.h"

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

#endif
