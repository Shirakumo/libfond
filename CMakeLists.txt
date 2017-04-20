cmake_minimum_required(VERSION 3.1)
project(fond)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_BUILD_TYPE Release)

include_directories(source)
add_library(fond SHARED "src/fond.c")
set_property(TARGET monitors PROPERTY C_STANDARD 99)

find_package(OpenGL REQUIRED)
target_link_libraries(fond ${OPENGL_LIBRARIES})

install(TARGETS fond DESTINATION /usr/lib)
