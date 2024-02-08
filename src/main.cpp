#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "engine.hpp"

#include <string>

using namespace matthewkayin;

enum AntAnimation {
    ANT_ANIMATION_IDLE,
    ANT_ANIMATION_WALK
};

int main() {
    if (!engine::init("Critter Farm", 640, 360)) {
        return -1;
    }
    engine::set_window_size(1280, 720);
    engine::Font font_small;
    font_small.load("./res/hack.ttf", 10);

    engine::Sprite ant_sprite;
    ant_sprite.load("./res/ant.png", 13, 3);
    ant_sprite.register_animation(ANT_ANIMATION_IDLE, 3, { ivec2(1, 1), ivec2(2, 1) });
    ant_sprite.register_animation(ANT_ANIMATION_WALK, 10, { ivec2(3, 1), ivec2(4, 1), ivec2(5, 1), ivec2(6, 1) });
    engine::SpriteAnimation ant_sprite_animation = engine::SpriteAnimation(&ant_sprite);
    engine::SpriteAnimation ant_sprite_animation2 = engine::SpriteAnimation(&ant_sprite);
    ant_sprite_animation.set_animation(ANT_ANIMATION_IDLE);
    ant_sprite_animation2.set_animation(ANT_ANIMATION_WALK);

    while (engine::running) {
        engine::timekeep();
        engine::poll_events();
        engine::render_clear();

        ant_sprite_animation.update(engine::delta);
        ant_sprite_animation2.update(engine::delta);

        ant_sprite_animation.render(vec2(64, 64));
        ant_sprite_animation2.render(vec2(96, 64));
        font_small.render("FPS: " + std::to_string(engine::fps), vec2(0.0f, 0.0f), engine::COLOR_WHITE);

        engine::render_flip();
    }
    
    engine::quit();
    return 0;
}