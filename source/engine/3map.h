//
// Created by ash on 10/12/22.
//

#ifndef SPLATOON_3DS_3MAP_H
#define SPLATOON_3DS_3MAP_H

#include "EntityType.h"
#include <cstdint>
#include <cstddef>

struct FileHeader {
    uint32_t models_count;
};
// Idea is for each one of these to correspond to one *model load*
// Since we might use the same model many times
struct FileModel {
    uint32_t entity_count;
    uint32_t entities_len;
    char name[16];
};
struct FileEntity {
    EntityType type;
    uint32_t attribs_length;
    std::byte attribs_value[];
};

#endif //SPLATOON_3DS_3MAP_H
