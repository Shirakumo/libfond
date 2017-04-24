cmake_minimum_required(VERSION 3.1)
project(fond)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(OpenGL REQUIRED)
find_package(GLEW)
find_package(GLFW)

if(GLFW_FOUND)
  include_directories(${GLFW_INCLUDE_DIRS})
endif()

if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIRS})
endif()

include_directories("src/")
include_directories(${OPENGL_INCLUDE_DIRS})

## Generate Shader Headers
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
    COMMAND sed -i 's/unsigned char/const GLchar/g' ${shader_hf}
    COMMAND sed -i 's/}\;/, 0x00}\;/g' ${shader_hf}
    DEPENDS ${shader_s}
    WORKING_DIRECTORY ${shader_dir}
    COMMENT "Generating header for ${shader_s}")
endforeach()

## Main Library
file(GLOB lib_src "src/*.c")
add_library(fond SHARED ${lib_src})
set_property(TARGET fond PROPERTY C_STANDARD 99)
if(WIN32)
  find_package(GLEW REQUIRED)
  target_link_libraries(fond ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})
else()
  target_link_libraries(fond ${OPENGL_LIBRARIES})
endif()

## Test Program
if(GLFW_FOUND AND GLEW_FOUND)
  file(GLOB test_src "test/*.c")
  add_executable(fond_test ${test_src})
  set_property(TARGET fond_test PROPERTY C_STANDARD 99)
  target_link_libraries(fond_test fond ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
endif()

## Warnings
if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
  add_definitions(/W2)
elseif(CMAKE_BUILD_TOOL MATCHES "(gcc|clang)")
  add_definitions(-W -Wall -Wextra -pedantic)
endif()

install(TARGETS fond DESTINATION /usr/local/lib)
