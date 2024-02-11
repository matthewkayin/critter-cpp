#include "engine.hpp"

#include <glad/glad.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <string>
#include <fstream>
#include <cmath>

using namespace siren;

bool Engine::init(const char* title, unsigned int screen_width, unsigned int screen_height) {
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
    this->screen_width = screen_width;
    this->screen_height = screen_height;
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

    if (!load_shader(&screen_shader, "./shader/screen.vs.glsl", "./shader/screen.fs.glsl")) {
        return false;
    }
    use_shader(screen_shader);
    set_shader_uniform("screen_size", vec2((float)screen_width, (float)screen_height));

    if (!load_shader(&text_shader, "./shader/text.vs.glsl", "./shader/text.fs.glsl")) {
        return false;
    }
    use_shader(text_shader);
    set_shader_uniform("screen_size", vec2((float)screen_width, (float)screen_height));

    if (!load_shader(&default_shader, "./shader/sprite.vs.glsl", "./shader/screen.fs.glsl")) {
        return false;
    }
    use_default_shader();
    set_shader_uniform("screen_size", vec2((float)screen_width, (float)screen_height));

    fps = 0;
    running = true;
    return true;
}

Engine::~Engine() {
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Engine::set_window_size(unsigned int window_width, unsigned int window_height) {
    this->window_width = window_width;
    this->window_height = window_height;
    SDL_SetWindowSize(window, window_width, window_height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Engine::timekeep() {
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

void Engine::poll_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
    }
}

/* Shader functions */

bool Engine::load_shader(Shader* id, const char* vertex_path, const char* fragment_path) {
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

    *id = glCreateProgram();
    glAttachShader(*id, shader[0]);
    glAttachShader(*id, shader[1]);
    glLinkProgram(*id);

    glGetProgramiv(*id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(*id, 512, nullptr, info_log);
        printf("Error linking shader program. Vertex: %s Fragment %s Error: %s\n", vertex_path, fragment_path, info_log);
    }

    glDeleteShader(shader[0]);
    glDeleteShader(shader[1]);

    return true;
}

void Engine::use_shader(Shader shader) {
    if (current_shader == shader) {
        return;
    }
    current_shader = shader;
    glUseProgram(current_shader);
}

void Engine::use_default_shader() {
    use_shader(default_shader);
}

void Engine::set_shader_uniform(const char* name, bool value) {
    glUniform1i(glGetUniformLocation(current_shader, name), value);
}

void Engine::set_shader_uniform(const char* name, unsigned int value) {
    glUniform1ui(glGetUniformLocation(current_shader, name), value);
}

void Engine::set_shader_uniform(const char* name, vec2 value) {
    glUniform2fv(glGetUniformLocation(current_shader, name), 1, value.value_ptr());
}

void Engine::set_shader_uniform(const char* name, ivec2 value) {
    glUniform2iv(glGetUniformLocation(current_shader, name), 1, value.value_ptr());
}

void Engine::set_shader_uniform(const char* name, Color value) {
    glUniform3fv(glGetUniformLocation(current_shader, name), 1, value.value_ptr());
}

/* Render functions */

void Engine::render_clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, screen_framebuffer);
    glViewport(0, 0, screen_width, screen_height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    use_default_shader();
}

void Engine::render_flip() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width, window_height);
    glBlendFunc(GL_ONE, GL_ZERO);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    use_shader(screen_shader);
    set_shader_uniform("sprite_texture", (unsigned int)0);
    set_shader_uniform("source_position", vec2());
    set_shader_uniform("source_size", vec2(screen_width, screen_height));
    set_shader_uniform("dest_position", vec2());
    set_shader_uniform("dest_size", vec2(screen_width, screen_height));

    glBindVertexArray(quad_vao);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(window);
}

void Engine::render_text(const Font& font, std::string text, vec2 position, Color color) {
    use_shader(text_shader);
    set_shader_uniform("sprite_texture", (unsigned int)0);
    set_shader_uniform("text_color", color);
    set_shader_uniform("source_size", vec2((float)font.glyph_width, (float)font.glyph_height));
    set_shader_uniform("dest_size", vec2((float)font.glyph_width, (float)font.glyph_height));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.atlas);
    glBindVertexArray(quad_vao);

    vec2 render_position = position;
    for (char c : text) {
        int glyph_index = (int)c - Font::FIRST_CHAR;
        set_shader_uniform("source_position", vec2((float)(font.glyph_width * glyph_index), 0));
        set_shader_uniform("dest_position", render_position);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        render_position.x += font.glyph_width;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Engine::render_sprite_animation(const SpriteAnimation& sprite_animation, vec2 position) {
    ivec2 sprite_frame = sprite_animation.sprite->animation_data[sprite_animation.animation].frames[sprite_animation.frame];
    render_sprite(*sprite_animation.sprite, position, sprite_frame.x, sprite_frame.y, sprite_animation.flip_h, sprite_animation.flip_v);
}

void Engine::render_sprite(const Sprite& sprite, vec2 position, unsigned int hframe, unsigned int vframe, bool flip_h, bool flip_v) {
    vec2 source_position = vec2(sprite.frame_width * hframe, sprite.frame_height * vframe);
    if (source_position.x + sprite.frame_width > sprite.width || source_position.y + sprite.frame_height > sprite.height) {
        printf("Sprite frame %u,%u is out of bounds\n", hframe, vframe);
        return;
    }
    vec2 frame_size = vec2((float)sprite.frame_width, (float)sprite.frame_height);

    set_shader_uniform("sprite_texture", (unsigned int)0);
    set_shader_uniform("dest_position", vec2(floorf(position.x), floorf(position.y)));
    set_shader_uniform("dest_size", frame_size);
    set_shader_uniform("source_position", source_position);
    set_shader_uniform("source_size", frame_size);
    set_shader_uniform("flip_h", flip_h);
    set_shader_uniform("flip_v", flip_v);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite.texture);
    glBindVertexArray(quad_vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}