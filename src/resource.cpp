#include "resource.hpp"

#include "engine.hpp"
#include "math.hpp"

Shader outline_shader;

Font font_small;

std::unordered_map<Tile, ivec2> tile_atlas_frame;
Sprite tileset;

Sprite ant_sprite;

bool resource_init() {
    Engine& engine = Engine::instance();

    bool success = true;

    success &= engine.load_shader(&outline_shader, "./shader/sprite.vs.glsl", "./shader/outline.fs.glsl");
    engine.use_shader(outline_shader);
    engine.set_shader_uniform("sprite_texture", (unsigned int)0);
    engine.set_shader_uniform("screen_size", vec2((float)engine.screen_width, (float)engine.screen_height));

    success &= font_small.load("./res/hack.ttf", 10);

    success &= tileset.load("./res/tiles.png", Sprite::SPECIFY_FRAME_SIZE, 32, 32);
    tile_atlas_frame[TILE_DIRT] = ivec2(14, 1);
    tile_atlas_frame[TILE_WATER] = ivec2(15, 1);

    success &= ant_sprite.load("./res/ant.png", Sprite::SPECIFY_FRAME_COUNT, 13, 3);
    ant_sprite.register_animation(ANT_ANIMATION_IDLE, 3, { ivec2(1, 1), ivec2(2, 1) });
    ant_sprite.register_animation(ANT_ANIMATION_WALK, 10, { ivec2(3, 1), ivec2(4, 1), ivec2(5, 1), ivec2(6, 1) });

    return success;
}