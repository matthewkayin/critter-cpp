#version 410 core

layout (location = 0) in vec2 vertex_position;

uniform vec2 screen_size;
uniform vec2 dest_position;
uniform vec2 dest_size;
uniform vec2 source_position;
uniform vec2 source_size;
uniform bool flip_h;
uniform bool flip_v;
uniform sampler2D sprite_texture;

out vec2 texture_coordinate;

void main() {
    vec2 position = dest_position + vec2(vertex_position.x * dest_size.x, vertex_position.y * dest_size.y);
    vec2 screen_space_position = (2.0 * vec2(position.x / screen_size.x, position.y / screen_size.y)) - vec2(1.0, 1.0);
    gl_Position = vec4(screen_space_position.x, screen_space_position.y, 0.0, 1.0);

    vec2 texture_size = vec2(textureSize(sprite_texture, 0));
    texture_coordinate = vertex_position;
    if (flip_h) {
        texture_coordinate.x = 1.0 - texture_coordinate.x;
    }
    if (flip_v) {
        texture_coordinate.y = 1.0 - texture_coordinate.y;
    }
    texture_coordinate = source_position + vec2(texture_coordinate.x * source_size.x, texture_coordinate.y * source_size.y);
    texture_coordinate = vec2(texture_coordinate.x / texture_size.x, texture_coordinate.y / texture_size.y);
}