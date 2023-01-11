#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "usings.h"

class Primitive;

class Material;

struct IntersectionPrimitive {
    Primitive *primitive;
    shared_ptr<Material> material;
    float tnear;
    float tfar;
    Vec3f normal;
    Vec3f p;
};

#endif
