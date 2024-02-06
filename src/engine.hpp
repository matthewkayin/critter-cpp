#pragma once

#include <glad/glad.h>

namespace matthewkayin::engine {
    extern unsigned int screen_width;
    extern unsigned int screen_height;

    extern unsigned int fps;
    extern float delta;
    extern bool running;

    bool init(const char* title, unsigned int _screen_width, unsigned int _screen_height);
    void quit();
    void set_window_size(unsigned int _window_width, unsigned int _window_height);
    void timekeep();
    void poll_events();

    /* Shader functions */
    struct Shader {
        GLuint id;
        bool load(const char* vertex_path, const char* fragment_path);
        void use();
    };

    /* Rendering functions */

    void render_clear();
    void render_flip();
}