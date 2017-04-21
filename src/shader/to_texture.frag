#version 330 core

uniform sampler2D tex_image;
uniform vec4 text_color = vec4(1.0, 1.0, 1.0, 1.0);

in vec2 tex_coord;
out vec4 color;

void main(){
  float intensity = texture(tex_image, tex_coord).r;
  color = vec4(intensity)*text_color;
}
