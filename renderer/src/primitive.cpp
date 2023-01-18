#include "primitive.h"

bool Primitive::intersect(const Ray& ray, IntersectionPrimitive& intersection) {
    if (shape->intersect(ray, intersection)) {
        intersection.primitive = this;
        intersection.material = this->material;
        return true;
    }
    return false;
}
