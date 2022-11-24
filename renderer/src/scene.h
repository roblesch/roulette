#pragma once

#include <map>
#include <memory>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "pugixml.hpp"

#include "primitive.h"

class Scene
{
public:
    Scene(
        std::map<std::string, std::shared_ptr<Material>> mat,
        std::map<std::string, std::shared_ptr<Primitive>> prim
    ) : materialMap(mat), primitiveMap(prim) {};

    static Scene FromMitsubaXML(const char* filename);

    std::map<std::string, std::shared_ptr<Material>> materialMap;
    std::map<std::string, std::shared_ptr<Primitive>> primitiveMap;
};
