#include "engine.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <string>
#include <fstream>

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

    // Renderbuffer
    static GLuint screen_framebuffer;
    static GLuint screen_texture;

    static Shader screen_shader;

    bool init(const char* title, unsigned int _screen_width, unsigned int _screen_height) {
        /* Setup SDL  */

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

        /* Setup Quad VAO */

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

        /* Setup screen framebuffer */
        
        glGenFramebuffers(1, &screen_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, screen_framebuffer);

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &screen_texture);
        glBindTexture(GL_TEXTURE_2D, screen_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        GLuint rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Screen framebuffer not complete!\n");
            return false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (!screen_shader.load("./shader/screen.vs.glsl", "./shader/screen.fs.glsl")) {
            return false;
        }

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

    void set_window_size(unsigned int _window_width, unsigned int _window_height) {
        window_width = _window_width;
        window_height = _window_height;
        SDL_SetWindowSize(window, window_width, window_height);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
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

        frames++;
    }

    void poll_events() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
    }

    /* Shader functions */

    bool Shader::load(const char* vertex_path, const char* fragment_path) {
        GLuint shader[2];
        std::string source[2];
        std::ifstream file[2];
        const char* path[2] = { vertex_path, fragment_path };
        static const GLenum type[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

        int success;
        char info_log[512];
        std::string line;

        for (int i = 0; i < 2; i++) {
            file[i].open(path[i]);
            if (!file[i].is_open()) {
                printf("Error opening shader at path %s\n", path[i]);
                return false;
            }

            while (std::getline(file[i], line)) {
                source[i] += line + "\n";
            }

            file[i].close();

            shader[i] = glCreateShader(type[i]);
            const char* source_cstr = source[i].c_str();
            glShaderSource(shader[i], 1, &source_cstr, nullptr);
            glCompileShader(shader[i]);

            glGetShaderiv(shader[i], GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader[i], 512, nullptr, info_log);
                printf("Error: shader %s failed to compile: %s\n", path[i], info_log);
                return false;
            }
        }

        id = glCreateProgram();
        glAttachShader(id, shader[0]);
        glAttachShader(id, shader[1]);
        glLinkProgram(id);

        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 512, nullptr, info_log);
            printf("Error linking shader program. Vertex: %s Fragment %s Error: %s\n", vertex_path, fragment_path, info_log);
        }

        glDeleteShader(shader[0]);
        glDeleteShader(shader[1]);

        return true;
    }

    void Shader::use() {
        glUseProgram(id);
    }

    /* Render functions */

    void render_clear() {
        glBindFramebuffer(GL_FRAMEBUFFER, screen_framebuffer);
        glViewport(0, 0, screen_width, screen_height);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_ONE, GL_ZERO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void render_flip() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_width, window_height);
        glBlendFunc(GL_ONE, GL_ZERO);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screen_shader.use();
        glBindVertexArray(quad_vao);
        glBindTexture(GL_TEXTURE_2D, screen_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }
}