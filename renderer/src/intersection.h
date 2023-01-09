#pragma once

#include "usings.h"

class Primitive;
class Material;

struct IntersectionPrimitive {
    Primitive* primitive;
    shared_ptr<Material> material;
    float tnear;
    float tfar;
    vec3f normal;
    vec3f p;
};