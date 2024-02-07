#pragma once

namespace matthewkayin {
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