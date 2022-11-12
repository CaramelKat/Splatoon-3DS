//
// Created by ash on 11/12/22.
//

#ifndef SPLATOON_3DS_COLOURSPACES_H
#define SPLATOON_3DS_COLOURSPACES_H

#include <cmath>

struct rgb {
    float r;
    float g;
    float b;

    static rgb fromHsv(float h, float s, float v) {
        return {
            v * mix(1.0, constrain(std::abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s),
            v * mix(1.0, constrain(std::abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s),
            v * mix(1.0, constrain(std::abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s)
        };
    }

private:
    static constexpr float fract(float x) { return x - int(x); }
    static constexpr float mix(float a, float b, float t) { return a + (b - a) * t; }
    static constexpr float step(float e, float x) { return x < e ? 0.0 : 1.0; }
    static constexpr float constrain(float val, float min, float max) { return std::min(max, std::max(val, min)); }
};

#endif //SPLATOON_3DS_COLOURSPACES_H
