#version 330 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 in_tex_coord;

out vec2 tex_coord;

void main(){
  gl_Position = vec4(position, 1.0);
  tex_coord = in_tex_coord;
}
