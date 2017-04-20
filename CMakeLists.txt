cmake_minimum_required(VERSION 3.1)
project(fond)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_BUILD_TYPE Release)

include_directories("src/")

file(GLOB_RECURSE shaders "src/shader/*")
foreach(shader_src ${shaders})
  set(shader_h ${shader_src}.h)
  set_source_files_properties(${shader_h} PROPERTIES GENERATED TRUE)
  add_custom_command(OUTPUT ${shader_h}
    COMMAND xdd -i ${shader_src} ${shader_h}
    COMMAND sed -i 's/};/,0x00};/g' ${shader_h}
    DEPENDS ${shader_src}
    COMMENT "Generating header for ${shader_src}")
endforeach()

file(GLOB_RECURSE sources "src/*.c")
add_library(fond SHARED ${sources})
set_property(TARGET fond PROPERTY C_STANDARD 99)

find_package(OpenGL REQUIRED)
target_link_libraries(fond ${OPENGL_LIBRARIES})

install(TARGETS fond DESTINATION /usr/lib)
