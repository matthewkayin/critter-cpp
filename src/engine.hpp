#pragma once

#include "math.hpp"
#include "color.hpp"
#include "font.hpp"
#include "sprite.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <initializer_list>
#include <unordered_map>

namespace siren {
    typedef GLuint Shader;

    class Engine {
    public:
        unsigned int screen_width;
        unsigned int screen_height;

        unsigned int fps;
        float delta;
        bool running;

        static Engine& instance() {
            static Engine engine;
            return engine;
        }
        bool init(const char* title, unsigned int screen_width, unsigned int screen_height);
        void set_window_size(unsigned int window_width, unsigned int window_height);
        void timekeep();
        void poll_events();

        /* Shaders */
        Shader default_shader;
        bool load_shader(Shader* id, const char* vertex_path, const char* fragment_path);
        void use_shader(Shader shader);
        void use_default_shader();
        void set_shader_uniform(const char* name, bool value);
        void set_shader_uniform(const char* name, unsigned int value);
        void set_shader_uniform(const char* name, vec2 value);
        void set_shader_uniform(const char* name, ivec2 value);
        void set_shader_uniform(const char* name, Color value);

        /* Rendering functions */
        void render_clear();
        void render_flip();
        void render_text(const Font& font, std::string text, vec2 position, Color color);
        
        void render_sprite_animation(const SpriteAnimation& sprite_animation, vec2 position);
        void render_sprite(const Sprite& sprite, vec2 position, unsigned int hframe = 0, unsigned int vframe = 0, bool flip_h = false, bool flip_v = false);

    private:
        SDL_Window* window;
        SDL_GLContext context;

        unsigned int window_width;
        unsigned int window_height;

        // Timekeeping
        const unsigned long FRAME_TIME = (unsigned long)(1000.0 / 60.0);
        unsigned long last_time;
        unsigned long last_second;
        unsigned int frames;

        // Quad VAO
        GLuint quad_vao;

        // Renderbuffer
        GLuint screen_framebuffer;
        GLuint screen_texture;

        Shader current_shader;
        Shader screen_shader;
        Shader text_shader;

        Engine() {}; // constructor
        ~Engine(); // destructor
        Engine(const Engine&) = delete; // copy constructor
        Engine(Engine&&) = delete; // move constructor
        void operator=(const Engine&) = delete; // copy assignment
        void operator=(Engine&&) = delete; // move assignment
    };
}