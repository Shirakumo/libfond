#version 330 core

uniform sampler2D tex_image;
uniform vec4 text_color = vec4(1);

in vec2 tex_coord;
out vec4 color;

void main(){
  color = texture(tex_image, tex_coord);
  color *= text_color;
}
