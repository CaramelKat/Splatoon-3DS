//
// Created by ash on 9/16/22.
//

#ifndef INC_3DS_GFX_3MDL_H
#define INC_3DS_GFX_3MDL_H

#include <cstdint>

enum MDL_DataType {
    MDL_FLAT = 0,
    MDL_INDEXED_U16 = 1,
};

typedef struct {
    uint32_t dtype;
    uint32_t indexes_sz;
    uint32_t vertexes_sz;
} MDL_Header;

#endif //INC_3DS_GFX_3MDL_H
