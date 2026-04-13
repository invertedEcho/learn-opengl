#version 330 core

out vec4 FragColor;

in vec3 ourColor;
// in vec3 ourPosition;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
  // mix linearly interpolates between our two textures.
  // third argument is weight, 0.2 will use 80% of first input and 20% of second input
  FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
