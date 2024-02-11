#pragma once

#include "math.hpp"

#include <glad/glad.h>
#include <vector>
#include <unordered_map>

namespace siren {
    struct Sprite {
        struct AnimationData {
            std::vector<ivec2> frames;
            float frame_duration;
        };

        GLuint texture;
        unsigned int width;
        unsigned int height;
        unsigned int frame_width;
        unsigned int frame_height;
        std::unordered_map<unsigned int, AnimationData> animation_data;

        enum FrameSizeOption {
            SPECIFY_FRAME_COUNT,
            SPECIFY_FRAME_SIZE
        };
        bool load(const char* path, FrameSizeOption frame_size_option = SPECIFY_FRAME_SIZE, unsigned int hframes = 1, unsigned int vframes = 1);
        void register_animation(unsigned int animation_name, int fps, const std::initializer_list<ivec2> &frames); 
    };

    struct SpriteAnimation {
        Sprite* sprite;
        unsigned int frame;
        unsigned int animation;
        bool flip_h;
        bool flip_v;
        float timer;

        SpriteAnimation() {};
        SpriteAnimation(Sprite* sprite);
        void update(unsigned int animation, float delta);
    };
}