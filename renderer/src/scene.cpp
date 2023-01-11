#include "scene.h"

void Scene::intersect(const Ray &ray, IntersectionPrimitive &intersection) const {
    for (const auto& pair : primitives) {
        pair.second->intersect(ray, intersection);
    }
}
