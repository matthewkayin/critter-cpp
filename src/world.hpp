#pragma once

#include "sprite.hpp"
#include "resource.hpp"

struct World {
    static const unsigned int MAP_WIDTH = 4;
    Tile map[MAP_WIDTH * MAP_WIDTH];

    vec2 camera_offset;

    siren::SpriteAnimation ant_animation;

    World();
    void update();
    void render();

    Tile map_get_tile(ivec2 coordinate) const;
    void map_set_tile(ivec2 coordinate, Tile value);
    vec2 map_to_world(const ivec2 map_coordinate) const;
};