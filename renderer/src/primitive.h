#include <string>

#include <glm/glm.hpp>
#include <utility>

class Material {
public:
    explicit Material(glm::vec3 rgb) : rgb(rgb) {};
    glm::vec3 rgb;
};

class Diffuse : public Material {
public:
    explicit Diffuse(glm::vec3 rgb) : Material(rgb) {};
};

class Emissive {
public:
    explicit Emissive(glm::vec3 radiance) : radiance(radiance) {};
    glm::vec3 radiance;
};

class Shape {
public:
    explicit Shape(glm::mat4 transform) : to_world(transform), to_obj(glm::inverse(transform)) {};
    glm::mat4 to_world;
    glm::mat4 to_obj;
};

class Rectangle : public Shape {
public:
    explicit Rectangle(glm::mat4 transform) : Shape(transform) {};

    glm::vec3 a{-1, -1, 0};
    glm::vec3 b{1, 1, 0};
    glm::vec3 normal{0, 0, 1};
};

class Cube : public Shape {
public:
    explicit Cube(glm::mat4 transform) : Shape(transform) {};

    glm::vec3 a{-1, -1, -1};
    glm::vec3 b{1, 1, 1};
};

class Primitive {
public:
    Primitive(std::shared_ptr<Shape> shape,
              std::shared_ptr<Material> material,
              std::shared_ptr<Emissive> emitter) :
        shape(std::move(shape)),
        material(std::move(material)),
        emitter(std::move(emitter)) {};

    std::shared_ptr<Shape> shape;
    std::shared_ptr<Material> material;
    std::shared_ptr<Emissive> emitter;
};
