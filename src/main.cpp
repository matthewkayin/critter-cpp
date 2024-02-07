#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "engine.hpp"

#include <string>

using namespace matthewkayin;

int main() {
    if (!engine::init("Critter Farm", 640, 360)) {
        return -1;
    }
    engine::set_window_size(1280, 720);
    engine::Font font_small;
    font_small.load("./res/hack.ttf", 10);

    engine::Sprite ant_sprite;
    ant_sprite.load("./res/ant.png", 13, 3);

    while (engine::running) {
        engine::timekeep();
        engine::poll_events();
        engine::render_clear();

        ant_sprite.render(vec2(64, 64), 1, 1, true);
        font_small.render("FPS: " + std::to_string(engine::fps), vec2(0.0f, 0.0f), engine::COLOR_WHITE);

        engine::render_flip();
    }
    
    engine::quit();
    return 0;
}