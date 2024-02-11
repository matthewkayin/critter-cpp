#pragma once

#include "sprite.hpp"

struct World {
    siren::SpriteAnimation ant_animation;

    World();
    void update();
    void render();
};