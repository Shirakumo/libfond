#version 330 core

in (layout = 0) vec3 position;
in (layout = 1) vec2 in_tex_coord;

out vec2 tex_coord;

void main(){
  gl_Position = position;
  tex_coord = in_tex_coord;
}
