# via https://github.com/cmmw/imgui-glfw-glad-glm/blob/master/glad/CMakeLists.txt
add_library(glad
    "src/glad.c"
    "include/glad/glad.h"
    "include/KHR/khrplatform.h"
)
target_include_directories(glad PUBLIC include)
