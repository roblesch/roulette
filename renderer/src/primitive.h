#include <string>

#include <glm/glm.hpp>

class Material
{
public:
    glm::vec3 rgb;
};

class Shape
{
public:
    glm::mat4 transform;
};

class Primitive
{
public:
    Shape shape;
    Material material;
};
