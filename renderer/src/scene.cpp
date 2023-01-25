#include "scene.h"

bool Scene::intersect(Ray &ray, IntersectionData &data) const {
    data.primitive = nullptr;
    data.material = nullptr;
    for (const auto& pair : primitives) {
        pair.second->intersect(ray, data);
    }
    if (data.primitive) {
        data.p = ray.p() + ray.d() * ray.tfar();
        data.w = ray.d();
        data.primitive->setIntersectionData(data);
        data.material = data.primitive->material;
        return true;
    }
    return false;
}
