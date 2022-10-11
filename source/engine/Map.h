//
// Created by ash on 10/11/22.
//

#ifndef INC_3DS_GFX_MAP_H
#define INC_3DS_GFX_MAP_H

#include <list>
#include <string>
#include "Entity.h"
#include "gfx/Model.h"

class Map {
    Map(const std::string& path);
private:
    std::list<Entity> m_entities;
    std::list<Model> m_models;
    // yet unknown "floor geometry" type
};

#endif //INC_3DS_GFX_MAP_H
