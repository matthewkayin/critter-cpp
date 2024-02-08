#version 410 core

in vec2 texture_coordinate;
out vec4 color;

uniform vec2 source_position;
uniform vec2 source_size;
uniform sampler2D sprite_texture;

uniform bool show_outline;

void main() {
    color = texture(sprite_texture, texture_coordinate);
    if (show_outline) {
        color = vec4(1.0);
    }
}