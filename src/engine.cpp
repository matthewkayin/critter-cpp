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
    static Shader text_shader;
    static Shader sprite_shader;

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
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,

            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
        };
        GLuint quad_vbo;

        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

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
        screen_shader.use();
        screen_shader.set_uniform("screen_size", vec2((float)screen_width, (float)screen_height));

        if (!text_shader.load("./shader/text.vs.glsl", "./shader/text.fs.glsl")) {
            return false;
        }
        text_shader.use();
        text_shader.set_uniform("screen_size", vec2((float)screen_width, (float)screen_height));

        if (!sprite_shader.load("./shader/sprite.vs.glsl", "./shader/screen.fs.glsl")) {
            return false;
        }
        sprite_shader.use();
        sprite_shader.set_uniform("screen_size", vec2((float)screen_width, (float)screen_height));

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

        delta = (float)(current_time - last_time) / 1000.0f;
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

    void Shader::set_uniform(const char* name, bool value) {
        glUniform1i(glGetUniformLocation(id, name), value);
    }

    void Shader::set_uniform(const char* name, unsigned int value) {
        glUniform1ui(glGetUniformLocation(id, name), value);
    }

    void Shader::set_uniform(const char* name, vec2 value) {
        glUniform2fv(glGetUniformLocation(id, name), 1, value.value_ptr());
    }

    void Shader::set_uniform(const char* name, Color value) {
        glUniform3fv(glGetUniformLocation(id, name), 1, value.value_ptr());
    }

    /* Font functions */

    static const int FIRST_CHAR = 32;

    int next_largest_power_of_two(int number) {
        int power_of_two = 1;
        while (power_of_two < number) {
            power_of_two *= 2;
        }

        return power_of_two;
    }

    bool Font::load(const char* path, unsigned int size) {
        static const SDL_Color SDL_COLOR_WHITE = { 255, 255, 255, 255 };

        // Load the font
        TTF_Font* ttf_font = TTF_OpenFont(path, size);
        if (ttf_font == NULL) {
            printf("Unable to open font at path %s. SDL Error: %s\n", path, TTF_GetError());
            return false;
        }

        // Render each glyph to a surface
        SDL_Surface* glyphs[96];
        int max_width;
        int max_height;
        for (int i = 0; i < 96; i++) {
            char text[2] = { (char)(i + FIRST_CHAR), '\0' };
            glyphs[i] = TTF_RenderText_Solid(ttf_font, text, SDL_COLOR_WHITE);
            if (glyphs[i] == NULL) {
                return false;
            }

            if (i == 0 || max_width < glyphs[i]->w) {
                max_width = glyphs[i]->w;
            }
            if (i == 0 || max_height < glyphs[i]->h) {
                max_height = glyphs[i]->h;
            }
        }

        int atlas_width = next_largest_power_of_two(max_width * 96);
        int atlas_height = next_largest_power_of_two(max_height);
        SDL_Surface* atlas_surface = SDL_CreateRGBSurface(0, atlas_width, atlas_height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        for (int i = 0; i < 96; i++) {
            SDL_Rect dest_rect = {  max_width * i, 0, glyphs[i]->w, glyphs[i]->h };
            SDL_BlitSurface(glyphs[i], NULL, atlas_surface, &dest_rect);
        }

        // Generate OpenGL texture
        glGenTextures(1, &atlas);
        glBindTexture(GL_TEXTURE_2D, atlas);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, atlas_surface->pixels);

        // Finish setting up font struct
        glyph_width = (unsigned int)max_width;
        glyph_height = (unsigned int)max_height;

        // Cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
        for (int i = 0; i < 96; i++) {
            SDL_FreeSurface(glyphs[i]);
        }
        SDL_FreeSurface(atlas_surface);
        TTF_CloseFont(ttf_font);

        // Determine font ID and insert it into font store
        return true;
    }

    void Font::render(std::string text, vec2 position, Color color) {
        text_shader.use();
        text_shader.set_uniform("sprite_texture", (unsigned int)0);
        text_shader.set_uniform("text_color", color);
        text_shader.set_uniform("source_size", vec2((float)glyph_width, (float)glyph_height));
        text_shader.set_uniform("dest_size", vec2((float)glyph_width, (float)glyph_height));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas);
        glBindVertexArray(quad_vao);

        vec2 render_position = position;
        for (char c : text) {
            int glyph_index = (int)c - FIRST_CHAR;
            text_shader.set_uniform("source_position", vec2((float)(glyph_width * glyph_index), 0));
            text_shader.set_uniform("dest_position", render_position);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            render_position.x += glyph_width;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /* Sprite functions */

    bool Sprite::load(const char* path, unsigned int hframes, unsigned int vframes) {
        SDL_Surface* surface = IMG_Load(path);
        if (surface == nullptr) {
            printf("Failed to load image %s\n", path);
            return false;
        }

        GLenum texture_format;
        switch (surface->format->BytesPerPixel) {
            case 1:
                texture_format = GL_RED;
                break;
            case 3:
                texture_format = GL_RGB;
                break;
            case 4:
                texture_format = GL_RGBA;
                break;
            default:
                printf("Format of texture %s not recognized\n", path);
                return false;
        }

        width = surface->w;
        height = surface->h;
        frame_width = width / hframes;
        frame_height = height / vframes;

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        SDL_FreeSurface(surface);

        return true;
    }

    void Sprite::render(vec2 position, unsigned int hframe, unsigned int vframe, bool flip_h, bool flip_v) {
        vec2 source_position = vec2(frame_width * hframe, frame_height * vframe);
        if (source_position.x + frame_width > width || source_position.y + frame_height > height) {
            printf("Sprite frame %u,%u is out of bounds\n", hframe, vframe);
            return;
        }
        vec2 frame_size = vec2((float)frame_width, (float)frame_height);

        sprite_shader.use();
        sprite_shader.set_uniform("sprite_texture", (unsigned int)0);
        sprite_shader.set_uniform("dest_position", position);
        sprite_shader.set_uniform("dest_size", frame_size);
        sprite_shader.set_uniform("source_position", source_position);
        sprite_shader.set_uniform("source_size", frame_size);
        sprite_shader.set_uniform("flip_h", flip_h);
        sprite_shader.set_uniform("flip_v", flip_v);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(quad_vao);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    void Sprite::register_animation(unsigned int animation_name, int fps, const std::initializer_list<ivec2> &frames) {
        animation_data[animation_name] = (AnimationData) {
            .frames = frames,
            .frame_duration = 1.0f / (float)fps
        };
    }

    SpriteAnimation::SpriteAnimation(Sprite* sprite) {
        this->sprite = sprite;
        frame = 0;
        animation = 0;
        flip_h = false;
        flip_v = false;
        timer = 0.0f;
    }

    void SpriteAnimation::update(float delta) {
        const Sprite::AnimationData& current_animation = sprite->animation_data.at(animation);

        timer += delta;
        if (timer >= current_animation.frame_duration) {
            timer -= current_animation.frame_duration;
            frame = (frame + 1) % current_animation.frames.size();
        }
    }

    void SpriteAnimation::set_animation(unsigned int animation_name) {
        animation = animation_name;
        frame = 0;
        timer = 0.0f;
    }

    void SpriteAnimation::render(vec2 position) {
        ivec2 sprite_frame = sprite->animation_data[animation].frames[frame];
        sprite->render(position, sprite_frame.x, sprite_frame.y, flip_h, flip_v);
    }

    /* Render functions */

    void render_clear() {
        glBindFramebuffer(GL_FRAMEBUFFER, screen_framebuffer);
        glViewport(0, 0, screen_width, screen_height);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void render_flip() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_width, window_height);
        glBlendFunc(GL_ONE, GL_ZERO);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screen_shader.use();
        screen_shader.set_uniform("sprite_texture", (unsigned int)0);
        screen_shader.set_uniform("source_position", vec2());
        screen_shader.set_uniform("source_size", vec2(screen_width, screen_height));
        screen_shader.set_uniform("dest_position", vec2());
        screen_shader.set_uniform("dest_size", vec2(screen_width, screen_height));

        glBindVertexArray(quad_vao);
        glBindTexture(GL_TEXTURE_2D, screen_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }
}