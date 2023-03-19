# don't offer suggestions on external code
set(CMAKE_CXX_CLANG_TIDY "")

add_subdirectory(json)
add_subdirectory(pugixml)
add_subdirectory(glfw)
add_subdirectory(glad)

# via https://github.com/cmmw/imgui-glfw-glad-glm/blob/master/imgui/CMakeLists.txt
add_library(
	imgui
	"imgui/imgui.cpp"
	"imgui/imgui.h"
	"imgui/imgui_demo.cpp"
	"imgui/imgui_draw.cpp"
	"imgui/imgui_widgets.cpp"
	"imgui/imgui_tables.cpp"
	"imgui/imgui_internal.h"
	"imgui/imstb_rectpack.h"
	"imgui/imstb_textedit.h"
	"imgui/imstb_truetype.h"
	"imgui/backends/imgui_impl_opengl3.cpp"
	"imgui/backends/imgui_impl_opengl3.h"
	"imgui/backends/imgui_impl_glfw.cpp"
	"imgui/backends/imgui_impl_glfw.h"
)
target_link_libraries(imgui PUBLIC glfw glad)
target_include_directories(imgui PUBLIC imgui imgui/backends)
