#include "scene.h"

bool Scene::intersect(const Ray &ray, IntersectionPrimitive &intersection) const {
    for (const auto& pair : primitives) {
        pair.second->intersect(ray, intersection);
    }
    if (intersection.primitive) {
        return true;
    }
    return false;
}
