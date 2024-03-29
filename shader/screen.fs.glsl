#version 410 core

in vec2 texture_coordinate;
out vec4 color;

uniform vec2 source_position;
uniform vec2 source_size;
uniform sampler2D sprite_texture;

void main() {
    color = texture(sprite_texture, texture_coordinate);
}