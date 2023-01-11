#ifndef SCENE_H
#define SCENE_H

#include "usings.h"

#include "camera.h"
#include "material.h"
#include "primitive.h"
#include "ray.h"

class Scene {
public:
    Scene() = default;

    Scene(Camera cam,
          unordered_map<string, shared_ptr<Material>> mats,
          unordered_map<string, shared_ptr<Primitive>> prims,
          unordered_map<string, shared_ptr<Primitive>> lights) :
            camera(cam),
            materials(std::move(mats)),
            primitives(std::move(prims)),
            lights(std::move(lights)) {};

    bool intersect(const Ray &ray, IntersectionPrimitive &intersection) const;

    Camera camera{};
    unordered_map<string, shared_ptr<Material>> materials;
    unordered_map<string, shared_ptr<Primitive>> primitives;
    unordered_map<string, shared_ptr<Primitive>> lights;
};

#endif
