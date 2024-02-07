#pragma once

#include "math.hpp"

#include <glad/glad.h>
#include <string>

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

    /* Color struct */
    struct Color {
        float r;
        float g;
        float b;
        Color() {
            r = 0.0f;
            g = 0.0f;
            b = 0.0f;
        }
        Color(int r, int g, int b) {
            this->r = (float)r / 255.0f;
            this->g = (float)g / 255.0f;
            this->b = (float)b / 255.0f;
        }
        float* value_ptr() {
            return &r;
        }
    };
    const Color COLOR_WHITE = Color(255, 255, 255);

    /* Shader functions */
    struct Shader {
        GLuint id;

        bool load(const char* vertex_path, const char* fragment_path);
        void use();
        void set_uniform(const char* name, bool value);
        void set_uniform(const char* name, unsigned int value);
        void set_uniform(const char* name, vec2 value);
        void set_uniform(const char* name, Color value);
    };

    /* Font functions */
    struct Font {
        GLuint atlas;
        unsigned int glyph_width;
        unsigned int glyph_height;

        bool load(const char* path, unsigned int size);
        void render(std::string text, vec2 position, Color color);
    };

    /* Sprite functions */

    struct Sprite {
        GLuint texture;
        unsigned int width;
        unsigned int height;
        unsigned int frame_width;
        unsigned int frame_height;

        bool load(const char* path, unsigned int hframes = 1, unsigned int vframes = 1);
        void render(vec2 position, unsigned int hframe = 0, unsigned int vframe = 0, bool flip_h = false, bool flip_v = false);
    };

    /* Rendering functions */
    void render_clear();
    void render_flip();
}