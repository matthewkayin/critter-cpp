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
    int font_small = engine::load_font("./res/hack.ttf", 10);

    while (engine::running) {
        engine::timekeep();
        engine::poll_events();
        engine::render_clear();

        engine::render_text(font_small, "FPS: " + std::to_string(engine::fps), vec2(0.0f, 0.0f), engine::COLOR_WHITE);

        engine::render_flip();
    }
    
    engine::quit();
    return 0;
}