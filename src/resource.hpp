#pragma once

#include "engine.hpp"
#include "sprite.hpp"
#include "font.hpp"

#include <unordered_map>

using namespace siren;

extern Font font_small;

extern Shader outline_shader;

enum Tile {
    TILE_NONE,
    TILE_DIRT,
    TILE_WATER
};

extern std::unordered_map<Tile, ivec2> tile_atlas_frame;
extern Sprite tileset;

enum AntAnimation {
    ANT_ANIMATION_IDLE,
    ANT_ANIMATION_WALK
};

extern Sprite ant_sprite;

bool resource_init();