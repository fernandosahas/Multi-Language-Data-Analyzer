cmake_minimum_required(VERSION 3.20)
project(analytics_gateway LANGUAGES CXX)

option(BUILD_PYTHON "Build Python extension via pybind11" ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 4.5 REQUIRED)

# pybind11 is header-only; prefer package if available, otherwise vendor via submodule.
if(BUILD_PYTHON)
  find_package(pybind11 CONFIG QUIET)
  if(NOT pybind11_FOUND)
    message(FATAL_ERROR "pybind11 not found. Install via vcpkg/conda/pip and provide CMAKE_PREFIX_PATH.")
  endif()
endif()

file(GLOB SRC_FILES
    src/video_source.cpp
    src/preprocessor.cpp
    src/inference_engine.cpp
    src/postprocessor.cpp
    src/pipeline.cpp)

add_library(analytics_core STATIC ${SRC_FILES})
target_include_directories(analytics_core PUBLIC ${OpenCV_INCLUDE_DIRS} src)
target_link_libraries(analytics_core PUBLIC ${OpenCV_LIBS})

add_executable(gateway_app src/analytics_gateway.cpp)
target_link_libraries(gateway_app PRIVATE analytics_core)

if(BUILD_PYTHON)
  pybind11_add_module(analytics bindings/py_module.cpp)
  target_link_libraries(analytics PRIVATE analytics_core)
  set_target_properties(analytics PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python/analytics")
endif()

install(TARGETS gateway_app analytics_core
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
