#version 330 core
uniform uvec2 size = uvec2(1, 1);

in layout(location = 0) vec2 position;
in layout(location = 1) vec2 in_tex_coord;
out vec2 tex_coord;

void main(){
  gl_Position = vec4((position.x/size.x)*2-1.0,
                     (position.y/size.y)*2+1.0,
                     0.0, 1.0);
  tex_coord = in_tex_coord;
}
