#include "scene.h"

bool Scene::intersect(Ray &ray, IntersectionData &intersection) const {
    for (const auto& pair : primitives) {
        pair.second->intersect(ray, intersection);
    }
    if (intersection.primitive) {
        intersection.p = ray.p() + ray.d() * ray.tfar();
        intersection.w = ray.d();
        intersection.primitive->setIntersectionData(ray, intersection);
        intersection.material = intersection.primitive->material;
        return true;
    }
    return false;
}
