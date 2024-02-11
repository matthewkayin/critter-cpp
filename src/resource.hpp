#pragma once

#include "engine.hpp"
#include "sprite.hpp"
#include "font.hpp"

extern siren::Font font_small;

extern siren::Shader outline_shader;

enum AntAnimation {
    ANT_ANIMATION_IDLE,
    ANT_ANIMATION_WALK
};

extern siren::Sprite ant_sprite;

bool resource_init();