//
// Created by ash on 9/16/22.
//

#ifndef INC_3DS_GFX_UTIL_H
#define INC_3DS_GFX_UTIL_H

#include <cstring>
#include <3ds/svc.h>
#define LOG(x) svcOutputDebugString(x, sizeof(x))
#define LOGF(fmt, ...) { \
    char _msgbuf[256];   \
    uint _sz = snprintf(_msgbuf, sizeof(_msgbuf)-1, fmt, __VA_ARGS__); \
    svcOutputDebugString(_msgbuf, (_sz > sizeof(_msgbuf) ? sizeof(_msgbuf) : _sz)); \
}

#endif //INC_3DS_GFX_UTIL_H
