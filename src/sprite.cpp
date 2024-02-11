#include "sprite.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>

using namespace siren;

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

void SpriteAnimation::update(unsigned int animation, float delta) {
    if (animation != this->animation) {
        this->animation = animation;
        frame = 0;
        timer = 0.0f;
    }

    const Sprite::AnimationData& current_animation = sprite->animation_data.at(animation);

    timer += delta;
    if (timer >= current_animation.frame_duration) {
        timer -= current_animation.frame_duration;
        frame = (frame + 1) % current_animation.frames.size();
    }
}