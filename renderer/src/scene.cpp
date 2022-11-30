#include "scene.h"

glm::vec3 as_vec3(const pugi::xml_node rgb)
{
    std::istringstream stream(rgb.attribute("value").value());
    std::string token;
    float buf[3];
    for (float & i : buf)
    {
        stream >> token;
        i = stof(token);
    }
    return glm::make_vec3(buf);
}

glm::mat4 as_mat4(const pugi::xml_node matrix)
{
    std::istringstream stream(matrix.attribute("value").value());
    std::string token;
    float buf[16];
    for (float & i : buf)
    {
        stream >> token;
        i = stof(token);
    }
    return glm::make_mat4(buf);
}

Scene Scene::FromMitsubaXML(const char* filename)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);

    std::shared_ptr<Camera> cam;
    std::map<std::string, std::shared_ptr<Material>> mat;
    std::map<std::string, std::shared_ptr<Primitive>> prim;

    if (result.status == pugi::status_ok)
    {
        pugi::xml_node scene = doc.child("scene");
        pugi::xml_node camera = scene.child("sensor");
        pugi::xml_object_range<pugi::xml_named_node_iterator> bsdfs = scene.children("bsdf");
        pugi::xml_object_range<pugi::xml_named_node_iterator> shapes = scene.children("shape");

        if (camera)
        {
            float fov = stof((std::string) camera.child("float").attribute("value").value());
            glm::mat4 transform = as_mat4(camera.child("transform").child("matrix"));
            cam = std::make_shared<Camera>(fov, transform);
        }
        
        for(pugi::xml_node bsdf : bsdfs)
        {
            std::string id = bsdf.attribute("id").value();
            glm::vec3 rgb = as_vec3(bsdf.child("bsdf").child("rgb"));
            mat.emplace(id, std::make_shared<Material>(rgb));
        }
        for (pugi::xml_node shape : shapes)
        {
            std::string id = shape.attribute("id").value();
            std::string matId = shape.child("ref").attribute("id").value();
            glm::mat4 transform = as_mat4(shape.child("transform").child("matrix"));
            prim.emplace(id, std::make_shared<Primitive>(std::make_shared<Shape>(transform), mat[matId]));
        }
    }

    return {cam, mat, prim};
}
