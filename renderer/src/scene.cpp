#include "scene.h"

bool Scene::intersect(const Ray &ray, IntersectionPrimitive &intersection) const {
    for (const auto& pair : primitives) {
        pair.second->intersect(ray, intersection);
    }
    return false;
}
