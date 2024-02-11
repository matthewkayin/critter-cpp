#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "resource.hpp"
#include "world.hpp"
#include "engine.hpp"

#include <string>

int main() {
    siren::Engine& engine = siren::Engine::instance();
    if (!engine.init("Critter Farm", 640, 360)) {
        return -1;
    }
    engine.set_window_size(1280, 720);

    if (!resource_init()) {
        return false;
    }

    World world;

    while (engine.running) {
        engine.timekeep();
        engine.poll_events();

        world.update();

        engine.render_clear();
        world.render();
        engine.render_text(font_small, "FPS: " + std::to_string(engine.fps), siren::vec2(0.0f, 0.0f), siren::COLOR_WHITE);

        engine.render_flip();
    }
    
    return 0;
}