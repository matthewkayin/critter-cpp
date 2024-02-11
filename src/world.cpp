#include "world.hpp"

#include "resource.hpp"
#include "engine.hpp"

World::World() {
    for (unsigned int i = 0; i < MAP_WIDTH * MAP_WIDTH; i++) {
        map[i] = TILE_WATER;
    }
    map_set_tile(ivec2(1, 1), TILE_DIRT);
    map_set_tile(ivec2(1, 2), TILE_DIRT);

    camera_offset = vec2(64.0f, 64.0f);

    ant_animation = siren::SpriteAnimation(&ant_sprite);
}

void World::update() {
    siren::Engine& engine = siren::Engine::instance();

    ant_animation.update(ANT_ANIMATION_WALK, engine.delta);
}

void World::render() {
    siren::Engine& engine = siren::Engine::instance();

    // Render map
    for (unsigned int row = 0; row < (MAP_WIDTH * 2) - 1; row++) {
        ivec2 coordinate = row < MAP_WIDTH ? ivec2(0, row) : ivec2(row - (MAP_WIDTH - 1), MAP_WIDTH - 1);
        unsigned int row_size = row < MAP_WIDTH ? coordinate.y + 1 : MAP_WIDTH - coordinate.x;
        for (unsigned int col = 0; col < row_size; col++) {
            Tile tile = map_get_tile(coordinate);
            if (tile != TILE_NONE) {
                ivec2 tile_frame = tile_atlas_frame[tile];
                engine.render_sprite(tileset, map_to_world(coordinate) + camera_offset, (unsigned int)tile_frame.x, (unsigned int)tile_frame.y);
            }

            coordinate.x++;
            coordinate.y--;
        }
    }

    engine.use_shader(outline_shader);
    engine.set_shader_uniform("show_outline", true);
    engine.render_sprite_animation(ant_animation, siren::vec2(64, 64));
}

Tile World::map_get_tile(ivec2 coordinate) const {
    return map[coordinate.x + (coordinate.y * MAP_WIDTH)];
}

void World::map_set_tile(ivec2 coordinate, Tile value) {
    map[coordinate.x + (coordinate.y * MAP_WIDTH)] = value;
}

vec2 World::map_to_world(const ivec2 map_coordinate) const {
    return (vec2(16.0f, 8.0f) * map_coordinate.x) + (vec2(-16.0f, 8.0f) * map_coordinate.y);
}