//
// Created by ash on 10/11/22.
//

#ifndef INC_3DS_GFX_ENTITY_H
#define INC_3DS_GFX_ENTITY_H

#include <cstdint>

#include "vector.h"
#include "gfx/Model.h"

enum EntityType : uint16_t {
    ENTITY_TYPE_STATIC_PROP = 0,
    ENTITY_TYPE_PHYSICS_PROP = 1,
    ENTITY_TYPE_PLAYER = 1000,
    ENTITY_TYPE_BOMB = 1001,
    // etc.
};

class Entity {
public:
    Entity(Model& model) : m_model(model) {}

    const std::string name;
    EntityType type;

    // todo pass in m_world_position, maybe texture
    void Draw(Gfx::State& gfx) { m_model.Draw(gfx); }
private:
    Model& m_model;
    vec3 m_world_position {0, 0, 0};
};

#endif //INC_3DS_GFX_ENTITY_H
