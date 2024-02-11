#pragma once

#include "math.hpp"
#include "color.hpp"
#include <glad/glad.h>
#include <string>

namespace siren {
    struct Font {
        static const int FIRST_CHAR = 32;

        GLuint atlas;
        unsigned int glyph_width;
        unsigned int glyph_height;

        bool load(const char* path, unsigned int size);
    };
}