#include "world.hpp"

#include "resource.hpp"
#include "engine.hpp"

World::World() {
    ant_animation = siren::SpriteAnimation(&ant_sprite);
}

void World::update() {
    siren::Engine& engine = siren::Engine::instance();

    ant_animation.update(ANT_ANIMATION_WALK, engine.delta);
}

void World::render() {
    siren::Engine& engine = siren::Engine::instance();

    engine.use_shader(outline_shader);
    engine.set_shader_uniform("show_outline", true);
    engine.render_sprite_animation(ant_animation, siren::vec2(64, 64));
}