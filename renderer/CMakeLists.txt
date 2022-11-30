cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 11)

project("renderer")

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ext/glm/CMakeLists.txt")
	message(FATAL_ERROR "Clone submodules
		$ git submodule update --init --recursive")
endif()


add_subdirectory("src")
add_subdirectory("ext")

add_executable(renderer
	"main.cpp"
)
target_link_libraries(renderer core)
