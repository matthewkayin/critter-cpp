#version 410 core

layout (location = 0) in vec2 vertex_position;

uniform vec2 screen_size;
uniform vec2 dest_position;
uniform vec2 dest_size;
uniform vec2 source_position;
uniform vec2 source_size;
uniform sampler2D sprite_texture;

out vec2 texture_coordinate;

void main() {
    vec2 position = dest_position + vec2(vertex_position.x * dest_size.x, vertex_position.y * dest_size.y);
    vec2 screen_space_position = (2.0 * vec2(position.x / screen_size.x, position.y / screen_size.y)) - vec2(1.0, 1.0);
    gl_Position = vec4(screen_space_position.x, screen_space_position.y, 0.0, 1.0);

    vec2 texture_size = vec2(textureSize(sprite_texture, 0));
    texture_coordinate = source_position + vec2(vertex_position.x * source_size.x, vertex_position.y * source_size.y);
    texture_coordinate = vec2(texture_coordinate.x / texture_size.x, 1.0 - ((texture_size.y - texture_coordinate.y) / texture_size.y));
}