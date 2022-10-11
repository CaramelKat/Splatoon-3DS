//
// Created by ash on 9/16/22.
//

#ifndef INC_SPLATOON_3DS_UTIL_H
#define INC_SPLATOON_3DS_UTIL_H

#include <cstring>
#include <cstdio>
#include <span>
#include <algorithm>
#include <functional>
#include <3ds/svc.h>

#define LOG(x) svcOutputDebugString(x, sizeof(x))
#define LOGF(fmt, ...) { \
    char _msgbuf[256];   \
    uint _sz = snprintf(_msgbuf, sizeof(_msgbuf)-1, fmt, __VA_ARGS__); \
    svcOutputDebugString(_msgbuf, (_sz > sizeof(_msgbuf) ? sizeof(_msgbuf) : _sz)); \
}

template <typename T>
inline T FromBytes(std::span<std::byte, sizeof(T)> bytes) {
    T ret;
    std::copy_n(bytes.begin(), sizeof(ret), std::as_writable_bytes(std::span(&ret, 1)).begin());
    return ret;
}

template <typename T>
inline bool ReadVector(std::vector<T>& vec, FILE* fd) {
    auto res = fread(vec.data(), sizeof(vec[0]), vec.size(), fd);
    return res == vec.size();
}

template <typename T>
inline bool ReadOne(T* ptr, FILE* fd) {
    auto res = fread(ptr, sizeof(*ptr), 1, fd);
    return res == 1;
}

class OnLeavingScope
{
public:
    // Use std::function so we can support
    // any function-like object
    using Func = std::function<void()>;

    // Prevent copying
    OnLeavingScope(const OnLeavingScope&) = delete;
    OnLeavingScope& operator=(const OnLeavingScope&) = delete;

    OnLeavingScope(const Func& f) :m_func(f) {}
    ~OnLeavingScope() { m_func(); }

private:
    Func m_func;
};

#endif //INC_SPLATOON_3DS_UTIL_H
