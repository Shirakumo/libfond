cmake_minimum_required(VERSION 3.1)
project(fond)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_BUILD_TYPE Release)

include_directories("src/")

add_custom_target(shaders ALL)
file(GLOB shaders "src/shader/*.vert" "src/shader/*.frag")
foreach(shader_s ${shaders})
  set(shader_h ${shader_s}.h)
  get_filename_component(shader_hf ${shader_h} NAME)
  get_filename_component(shader_sf ${shader_s} NAME)
  get_filename_component(shader_dir ${shader_h} DIRECTORY)
  set_source_files_properties(${shader_h} PROPERTIES GENERATED TRUE)
  add_custom_command(
    TARGET shaders
    COMMAND xxd -i ${shader_sf} ${shader_hf}
    DEPENDS ${shader_s}
    WORKING_DIRECTORY ${shader_dir}
    COMMENT "Generating header for ${shader_s}")
endforeach()

file(GLOB_RECURSE sources "src/*.c")
add_library(fond SHARED ${sources})
set_property(TARGET fond PROPERTY C_STANDARD 99)

if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
  add_definitions(/W2)
elseif(CMAKE_BUILD_TOOL MATCHES "(gcc|clang)")
  add_definitions(-W -Wall -Wextra -pedantic)
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(fond ${OPENGL_LIBRARIES})

install(TARGETS fond DESTINATION /usr/lib)
