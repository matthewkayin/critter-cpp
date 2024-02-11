#pragma once

namespace siren {
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
}