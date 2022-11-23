#pragma once

#include <map>
#include <memory>
#include <string>

class Material;
class Primitive;

class Scene
{
public:
    static Scene FromMitsubaXML(const char* filename);

public:
    std::map<std::string, std::shared_ptr<Material>> materialMap;
    std::map<std::string, std::shared_ptr<Primitive>> primitiveMap;
};
