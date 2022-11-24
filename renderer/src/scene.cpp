#include "scene.h"

glm::vec3 as_vec3(const pugi::char_t *value)
{
    std::istringstream stream(value);
    std::string token;
    float buf[3];
    for (int i = 0; i < 3; i++)
    {
        stream >> token;
        buf[i] = stof(token);
    }
    return glm::make_vec3(buf);
}

glm::mat4 as_mat4(const pugi::char_t* value)
{
    std::istringstream stream(value);
    std::string token;
    float buf[16];
    for (int i = 0; i < 16; i++)
    {
        stream >> token;
        buf[i] = stof(token);
    }
    return glm::make_mat4(buf);
}

Scene Scene::FromMitsubaXML(const char* filename)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);

    std::map<std::string, std::shared_ptr<Material>> mat;
    std::map<std::string, std::shared_ptr<Primitive>> prim;

    if (result.status == pugi::status_ok)
    {
        pugi::xml_object_range<pugi::xml_named_node_iterator> bsdfs = doc.child("scene").children("bsdf");
        pugi::xml_object_range<pugi::xml_named_node_iterator> shapes = doc.child("scene").children("shape");
        
        for(pugi::xml_node bsdf : bsdfs)
        {
            std::string id = bsdf.attribute("id").value();
            glm::vec3 rgb = as_vec3(bsdf.child("bsdf").child("rgb").attribute("value").value());
        }
        for (pugi::xml_node shape : shapes)
        {
            glm::mat4 transform = as_mat4(shape.child("transform").child("matrix").attribute("value").value());
        }
    }

    return Scene(mat, prim);
}
