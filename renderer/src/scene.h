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

class Camera
{
public:
    Camera(float fov, glm::mat4 transform) : fov(fov), to_world(transform) {};

    float fov;
    glm::mat4 to_world;
};

class Scene
{
public:
    Scene(
        std::shared_ptr<Camera> cam,
        std::map<std::string, std::shared_ptr<Material>> mat,
        std::map<std::string, std::shared_ptr<Primitive>> prim
    ) : camera(std::move(cam)), materialMap(std::move(mat)), primitiveMap(std::move(prim)) {};

    static Scene FromMitsubaXML(const char* filename);

    std::shared_ptr<Camera> camera;
    std::map<std::string, std::shared_ptr<Material>> materialMap;
    std::map<std::string, std::shared_ptr<Primitive>> primitiveMap;
};
