#include "primitive.h"

bool Primitive::intersect(Ray& ray, IntersectionData& intersection) {
    if (shape->intersect(ray, intersection)) {
        intersection.primitive = this;
        return true;
    }
    return false;
}
