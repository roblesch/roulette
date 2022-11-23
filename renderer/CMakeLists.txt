cmake_minimum_required(VERSION 3.8)

project("renderer")

add_subdirectory("src")
add_executable(renderer
	"main.cpp"
)

target_link_libraries(renderer core)
