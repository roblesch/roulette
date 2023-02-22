cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)

set(CMAKE_OSX_ARCHITECTURES x86_64)

project("renderer")

if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ext/json/CMakeLists.txt")
    message(FATAL_ERROR "Clone submodules
        $ git submodule update --init --recursive")
endif()

set(STB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext/stb")
include_directories(${STB_INCLUDE_DIR})
link_directories(${STB_INCLUDE_DIR})

add_subdirectory("src")
add_subdirectory("ext")

add_executable(renderer
    "main.cpp"
)

target_link_libraries(renderer core)

if (WIN32)
    add_custom_command(TARGET renderer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OIDN_BIN_DIR}/OpenImageDenoise.dll"
            $<TARGET_FILE_DIR:renderer>)
    add_custom_command(TARGET renderer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TBB_BIN_DIR}/tbb12.dll"
            $<TARGET_FILE_DIR:renderer>)
else()
    add_custom_command(TARGET renderer POST_BUILD
            COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${TBB_LIB_DIR}"
            $<TARGET_FILE:renderer>)
endif()
