#version 410 core

in vec2 texture_coordinate;
out vec4 color;

uniform sampler2D sprite_texture;
uniform vec3 text_color;

void main() {
    color = vec4(text_color, texture(sprite_texture, texture_coordinate).r);
}