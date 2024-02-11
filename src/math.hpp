#pragma once

namespace siren {
    template<typename T>
    struct vec {
        T x;
        T y;

        vec() {}
        vec(T x, T y) {
            this->x = x;
            this->y = y;
        }
        vec(const vec& other) {
            x = other.x;
            y = other.y;
        }
        T* value_ptr() {
            return &x;
        }
        vec operator+(const vec& other) const {
            return vec(x + other.x, y + other.y);
        }
        vec operator-(const vec& other) const {
            return vec(x - other.x, y - other.y);
        }
        vec operator*(const float scaler) const {
            return vec(x * scaler, y * scaler);
        }
    };

    typedef vec<int> ivec2;
    typedef vec<float> vec2;
}