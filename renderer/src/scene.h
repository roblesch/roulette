#ifndef SCENE_H
#define SCENE_H

#include "usings.h"

#include "aabb.h"
#include "camera.h"
#include "material.h"
#include "primitive.h"
#include "ray.h"

class Scene {
public:
    Scene() = default;

    Scene(Camera cam,
          AABB aabb,
          unordered_map<std::string, shared_ptr<Material>> mats,
          unordered_map<std::string, shared_ptr<Primitive>> prims,
          unordered_map<std::string, shared_ptr<Primitive>> lights) :
            camera(cam),
            bounds(aabb),
            materials(std::move(mats)),
            primitives(std::move(prims)),
            lights(std::move(lights)) {};

    bool intersect(Ray& ray, Intersection& intersection, IntersectionData& data) const {
        intersection.primitive = nullptr;
        data.primitive = nullptr;
        for (const auto& pair : primitives) {
            pair.second->intersect(ray, intersection);
        }
        if (intersection.primitive) {
            data.p = ray.p() + ray.d() * ray.tfar();
            data.w = ray.d();
            data.epsilon = F_NEAR_ZERO;
            intersection.primitive->setIntersectionData(intersection, data);
            return true;
        }
        return false;
    }

    Camera camera{};
    AABB bounds;
    unordered_map<std::string, shared_ptr<Material>> materials;
    unordered_map<std::string, shared_ptr<Primitive>> primitives;
    unordered_map<std::string, shared_ptr<Primitive>> lights;
};

#endif
