cmake_minimum_required(VERSION 3.10) 

project(FractalBox)

set(CMAKE_BUILD_TYPE Debug)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ext")
file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# add_definitions(-DGLM_ENABLE_EXPERIMENTAL) # for glm/gtx/vec_swizzle.hpp

set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

target_include_directories(${PROJECT_NAME} PRIVATE "${LIB_DIR}")

find_package(Curses)
if (CURSES_FOUND)
    include_directories(${CURSES_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})
else ()
    message(FATAL_ERROR "Error: Could not locate curses/ncurses library")
endif ()

add_compile_definitions(SHADER_PATH=${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Adds 'run' target to makefile
add_custom_target(run
    COMMAND "${PROJECT_NAME}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

add_custom_target(dbg
    COMMAND gdb ${PROJECT_NAME}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    VERBATIM)
