#version 410 core

in vec2 texture_coordinate;
out vec4 color;

uniform vec2 source_position;
uniform vec2 source_size;
uniform sampler2D sprite_texture;

uniform bool show_outline;

void main() {
    vec2 texture_size = textureSize(sprite_texture, 0);
    vec2 pixel_size = vec2(1.0 / texture_size.x, 1.0 / texture_size.y);

    // sample each pixel that is a single pixel outside the current texture coordinate
    float outline = 0.0;
    outline += texture(sprite_texture, texture_coordinate + vec2(-pixel_size.x, 0.0)).a;
    outline += texture(sprite_texture, texture_coordinate + vec2(0.0, pixel_size.y)).a;
    outline += texture(sprite_texture, texture_coordinate + vec2(pixel_size.x, 0.0)).a;
    outline += texture(sprite_texture, texture_coordinate + vec2(0.0, -pixel_size.y)).a;
    outline += texture(sprite_texture, texture_coordinate + vec2(-pixel_size.x, -pixel_size.y)).a;
    outline += texture(sprite_texture, texture_coordinate + vec2(pixel_size.x, -pixel_size.y)).a;
    outline += texture(sprite_texture, texture_coordinate + vec2(-pixel_size.x, pixel_size.y)).a;
    outline += texture(sprite_texture, texture_coordinate + vec2(pixel_size.x, pixel_size.y)).a;
    // this min turns our result into a binary 1 or 0. 1 = there is another pixel next to us, 0 = there isn't
    outline = min(outline, 1.0);

    color = texture(sprite_texture, texture_coordinate);
    if (show_outline) {
        color = mix(color, vec4(1.0), outline - color.a);
    }
}