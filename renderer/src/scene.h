#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include "pugixml.hpp"

#include "primitive.h"

class Ray {
public:
    Ray(glm::vec3 o, glm::vec3 d) : o(o), d(glm::normalize(d)), t(0) {};
    glm::vec3 at(float dt) const { return o + d * dt; }

    glm::vec3 o;
    glm::vec3 d;
    float t;
};

class Camera {
public:
    Camera(float fov, glm::mat4 transform) : fov(fov), to_world(transform) {};

    float fov;
    glm::mat4 to_world;
};

class Scene {
public:
    Scene(std::shared_ptr<Camera> cam,
          std::map<std::string, std::shared_ptr<Material>> mats,
          std::map<std::string, std::shared_ptr<Primitive>> prims) :
        camera(std::move(cam)),
        materials(std::move(mats)),
        primitives(std::move(prims)) {};

    static Scene FromMitsubaXML(const char *filename);

    std::shared_ptr<Camera> camera;
    std::map<std::string, std::shared_ptr<Material>> materials;
    std::map<std::string, std::shared_ptr<Primitive>> primitives;
};
