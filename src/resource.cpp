#include "resource.hpp"

#include "engine.hpp"
#include "math.hpp"

siren::Shader outline_shader;

siren::Font font_small;

siren::Sprite ant_sprite;

bool resource_init() {
    siren::Engine& engine = siren::Engine::instance();

    bool success = true;

    success &= engine.load_shader(&outline_shader, "./shader/sprite.vs.glsl", "./shader/outline.fs.glsl");
    engine.use_shader(outline_shader);
    engine.set_shader_uniform("sprite_texture", (unsigned int)0);
    engine.set_shader_uniform("screen_size", siren::vec2((float)engine.screen_width, (float)engine.screen_height));

    success &= font_small.load("./res/hack.ttf", 10);

    success &= ant_sprite.load("./res/ant.png", 13, 3);
    ant_sprite.register_animation(ANT_ANIMATION_IDLE, 3, { siren::ivec2(1, 1), siren::ivec2(2, 1) });
    ant_sprite.register_animation(ANT_ANIMATION_WALK, 10, { siren::ivec2(3, 1), siren::ivec2(4, 1), siren::ivec2(5, 1), siren::ivec2(6, 1) });

    return success;
}