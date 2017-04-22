#version 330 core
uniform vec4 extent = vec4(1.0);

in layout(location = 0) vec2 position;
in layout(location = 1) vec2 in_tex_coord;
out vec2 tex_coord;

void main(){
  gl_Position = vec4(2*(position.x+extent.x)/extent.z-1.0,
                     2*(position.y-extent.y)/extent.w+1.0,
                     0.0, 1.0);
  tex_coord = in_tex_coord;
}
