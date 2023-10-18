//
// Created by ash on 10/19/23.
//

#ifndef SPLATOON_3DS_ENTPLAYER_H
#define SPLATOON_3DS_ENTPLAYER_H

#include "engine/Entity.h"

class EntPlayer : public Entity {
public:
    EntPlayer(Model& model, std::string name);

    // todo this sucks
    void move(vec3 off) {
        m_world_position += off;
    }

    [[nodiscard]] vec3 position() const {
        return m_world_position;
    }
};


#endif //SPLATOON_3DS_ENTPLAYER_H
