cmake_minimum_required(VERSION 3.10)
project(ToonShading)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIRS})

add_executable(ToonShading src/main.cpp)

target_link_libraries(ToonShading ${OPENGL_LIBRARIES})
target_link_libraries(ToonShading glfw)
# target_link_libraries(ToonShading /opt/homebrew/opt/glfw/lib/libglfw3.a)
target_link_libraries(ToonShading GLEW::GLEW)
# target_link_libraries(ToonShading /opt/homebrew/opt/glew/lib/libGLEW.a)