#pragma once

#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include "pugixml.hpp"

using std::ifstream;
using std::istringstream;
using std::map;
using std::make_shared;
using std::shared_ptr;
using std::string;

using glm::vec3;
using glm::vec4;
using glm::make_mat4;
using glm::make_vec3;
using glm::mat4;
using glm::normalize;

using nlohmann::json;

using pugi::xml_document;
using pugi::xml_named_node_iterator;
using pugi::xml_node;
using pugi::xml_object_range;
using pugi::xml_parse_result;
using pugi::status_ok;

