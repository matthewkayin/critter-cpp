#pragma once

namespace siren {
    struct ivec2 {
        int x;
        int y;
        ivec2() {
            x = 0;
            y  =0;
        }
        ivec2(int x, int y) {
            this->x = x;
            this->y = y;
        }
        int* value_ptr() {
            return &x;
        }
    };

    struct vec2 {
        float x;
        float y;
        vec2() {
            x = 0.0f;
            y = 0.0f;
        }
        vec2(float x, float y) {
            this->x = x;
            this->y = y;
        }
        float* value_ptr() {
            return &x;
        }
    };
}