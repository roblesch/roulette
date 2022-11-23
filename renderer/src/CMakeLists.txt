cmake_minimum_required(VERSION 3.8)

include_directories(
  ${PUGIXML_INCLUDE_DIRS}
)

add_library(pugixml
	pugixml.cpp
	pugixml.hpp
	pugiconfig.hpp
)

add_library(core
	scene_parser.cpp
	scene_parser.h
)

target_link_libraries(core
	pugixml
)
