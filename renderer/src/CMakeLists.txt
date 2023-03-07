set(ONEAPI_ROOT "")
if(DEFINED ENV{ONEAPI_ROOT})
  set(ONEAPI_ROOT "$ENV{ONEAPI_ROOT}")
  message(STATUS "ONEAPI_ROOT FROM ENVIRONMENT: ${ONEAPI_ROOT}")
else()
  if(WIN32)
    set(ONEAPI_ROOT "C:/Program Files (x86)/Intel/oneAPI")
  else()
    set(ONEAPI_ROOT /opt/intel/oneapi)
  endif()
  message(STATUS "ONEAPI_ROOT DEFAULT: ${ONEAPI_ROOT}")
endif(DEFINED ENV{ONEAPI_ROOT})
set(OIDN_ROOT ${ONEAPI_ROOT}/oidn/latest)
set(OIDN_INCLUDE_DIR ${OIDN_ROOT}/include)
set(OIDN_LIB_DIR ${OIDN_ROOT}/lib)

if (WIN32)
    set(OIDN_BIN_DIR ${OIDN_ROOT}/bin PARENT_SCOPE)
    set(TBB_BIN_DIR ${ONEAPI_ROOT}/tbb/latest/redist/intel64/vc14 PARENT_SCOPE)
else()
    set(TBB_LIB_DIR ${ONEAPI_ROOT}/tbb/latest/lib PARENT_SCOPE)
endif()

find_package(OpenImageDenoise REQUIRED PATHS ${ONEAPI_ROOT})
include_directories(${OIDN_INCLUDE_DIR} ${OIDN_ROOT})
link_directories(${OIDN_LIB_DIR})

add_library(core
        "earstracer.cpp"
        "camera.h"
        "framebuffer.h"
        "framebuffer.cpp"
        "integrator.h"
        "integrator.cpp"
        "intersection.h"
        "material.h"
        "ears.h"
        "pathtracer.cpp"
        "primitive.h"
        "ray.h"
        "raytracer.cpp"
        "renderer.h"
        "samplebuffer.h"
        "sampler.h"
        "scene.h"
        "sceneparser.h"
        "sceneparser.cpp"
        "shape.h"
        "shape.cpp"
        "tracer.h"
        "tungstenmath.h"
        "usings.h")

target_link_libraries(core
        nlohmann_json
        pugixml
        OpenImageDenoise)
