#pragma once

namespace matthewkayin::engine {
    extern unsigned int screen_width;
    extern unsigned int screen_height;

    extern unsigned int fps;
    extern float delta;
    extern bool running;

    bool init(const char* title, unsigned int _screen_width, unsigned int _screen_height);
    void quit();
    void timekeep();
    void poll_events();
}