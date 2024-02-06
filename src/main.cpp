#ifdef _WIN32
    #define SDL_MAIN_HANDLED
#endif

#include "engine.hpp"

using namespace matthewkayin;

int main() {
    if (!engine::init("Critter Farm", 640, 360)) {
        return -1;
    }

    while (engine::running) {
        engine::timekeep();
        engine::poll_events();
    }
    
    engine::quit();
    return 0;
}