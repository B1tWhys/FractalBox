cmake_minimum_required(VERSION 2.6)

project(FractalBox)
file(GLOB SOURCES "src/*.cpp")
include_directories(include)
add_executable(FractalBox ${SOURCES})

add_subdirectory(ext/glfw-3.2.1)
include_directories(ext/glfw-3.2.1/include)
target_link_libraries(FractalBox glfw ${GLFW_LIBRARIES})
