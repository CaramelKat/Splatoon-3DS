//
// Created by ash on 10/12/22.
//

#ifndef SPLATOON_3DS_ENTITYTYPE_H
#define SPLATOON_3DS_ENTITYTYPE_H

#include <cstdint>

enum EntityType : uint16_t {
    ENTITY_TYPE_STATIC_PROP = 0,
    ENTITY_TYPE_PHYSICS_PROP = 1,
    ENTITY_TYPE_PLAYER = 1000,
    ENTITY_TYPE_BOMB = 1001,
    // etc.
};

#endif //SPLATOON_3DS_ENTITYTYPE_H
