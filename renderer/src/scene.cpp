#include "scene.h"

#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "pugixml.hpp"

glm::vec3 as_vec3(const pugi::char_t *value)
{
    float buf[3];
    std::string str = value;
    std::string::size_type t;
    buf[0] = stof(str, &t);
    buf[1] = stof(str.substr(t + 1), &t);
    buf[2] = stof(str.substr(t + 1));
    return glm::make_vec3(buf);
}

glm::mat4 as_mat4(const pugi::char_t* value)
{
    // note: check out isstringstream
    //double buf[16];
    //std::string str = value;
    //std::string::size_type t;
    //buf[0] = stof(str, &t);
    //for (int i = 1; i < 16; i++)
    //{
    //    buf[i] = stof(str.substr(t), &t);
    //}
    //return glm::make_mat4(buf);
    return glm::mat4();
}

Scene Scene::FromMitsubaXML(const char* filename)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename);
    if (result.status == pugi::status_ok)
    {
        pugi::xml_object_range<pugi::xml_named_node_iterator> bsdfs = doc.child("scene").children("bsdf");
        pugi::xml_object_range<pugi::xml_named_node_iterator> shapes = doc.child("scene").children("shape");
        
        for(pugi::xml_node bsdf : bsdfs)
        {
            glm::vec3 rgb = as_vec3(bsdf.child("bsdf").child("rgb").attribute("value").value());
        }
        for (pugi::xml_node shape : shapes)
        {
            glm::mat4 tsf = as_mat4(shape.child("transform").child("matrix").attribute("value").value());
        }
    }
    return Scene();
}
