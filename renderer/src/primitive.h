#include <string>

#include <glm/glm.hpp>
#include <utility>

class Ray
{
public:
    Ray(glm::vec3 o, glm::vec3 d) : o(o), d(glm::normalize(d)), t(0) {};
    glm::vec3 at(float dt) const { return o + d * dt; }

    glm::vec3 o;
    glm::vec3 d;
    float t;
};

class Material
{
public:
    explicit Material(glm::vec3 rgb) : rgb(rgb) {};
    glm::vec3 rgb;
};

class Shape
{
public:
    explicit Shape(glm::mat4 transform) : to_world(transform), to_obj(glm::inverse(transform)) {};

    glm::mat4 to_world;
    glm::mat4 to_obj;
};

class Rectangle : Shape
{
public:
};

class Primitive
{
public:
    Primitive(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material)
        : shape(std::move(shape)), material(std::move(material)) {};

    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;
};
