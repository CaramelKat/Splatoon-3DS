//
// Created by ash on 10/19/23.
//

#include "EntPlayer.h"

#include <utility>

EntPlayer::EntPlayer(Model &model, std::string name) : Entity(model, std::move(name)) {
    m_world_scale = {0.1, 0.1, 0.1}; //miku is quite big
}
