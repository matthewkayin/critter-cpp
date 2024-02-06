#include "engine.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>

namespace matthewkayin::engine {
    static SDL_Window* window;
    static SDL_GLContext context;

    unsigned int screen_width;
    unsigned int screen_height;
    static unsigned int window_width;
    static unsigned int window_height;

    // Timekeeping
    static const unsigned long FRAME_TIME = (unsigned long)(1000.0 / 60.0);
    static unsigned long last_time;
    static unsigned long last_second;
    unsigned int fps;
    float delta = 0.0f;
    static unsigned int frames = 0;
    bool running = false;

    // Quad VAO
    static GLuint quad_vao;

    bool init(const char* title, unsigned int _screen_width, unsigned int _screen_height) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("Error initializing SDL: %s\n", SDL_GetError());
            return false;
        }

        int img_flags = IMG_INIT_PNG;
        if (!(IMG_Init(img_flags) & img_flags)) {
            printf("Error initializing SDL_image: %s\n", IMG_GetError());
            return false;
        }

        if (TTF_Init() == -1) {
            printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
            return false;
        }

        // Set GL version
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_LoadLibrary(nullptr);

        // Create SDL window
        screen_width = _screen_width;
        screen_height = _screen_height;
        window_width = screen_width;
        window_height = screen_height;
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL);
        if (window == nullptr) {
            printf("Error creating window: %s\n", SDL_GetError());
            return false;
        }

        // Create GL context
        context = SDL_GL_CreateContext(window);
        if (context == nullptr) {
            printf("Error creating GL context: %s\n", SDL_GetError());
            return false;
        }

        // Setup GLAD
        gladLoadGLLoader(SDL_GL_GetProcAddress);
        if (glGenVertexArrays == nullptr) {
            printf("Error loading OpenGL.\n");
            return false;
        }

        // Setup Quad VAO
        float quad_vertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
        };
        GLuint quad_vbo;

        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);

        fps = 0;
        running = true;

        return true;
    }

    void quit() {
        TTF_Quit();
        IMG_Quit();
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void timekeep() {
        unsigned long current_time = SDL_GetTicks();
        while (current_time - last_time < FRAME_TIME) {
            current_time = SDL_GetTicks();
        }

        delta = (float)(current_time - last_time) / 60.0f;
        last_time = current_time;

        if (current_time - last_second >= 1000) {
            fps = frames;
            frames = 0;
            last_second += 1000;
        }
    }

    void poll_events() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
    }
}