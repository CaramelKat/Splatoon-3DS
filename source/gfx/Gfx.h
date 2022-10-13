//
// Created by ash on 9/16/22.
//

#ifndef INC_SPLATOON_3DS_GFX_H
#define INC_SPLATOON_3DS_GFX_H

#include <citro3d.h>

namespace Gfx {
    const bool left_hand = false;
    const C3D_FVec Up = FVec3_New(0.0f, 1.0f, 0.0f);

    class State {
    public:
        State();
        ~State();
        void Update() {
            Mtx_LookAt(&view_matrix, t.camera_position, t.camera_target, Up, left_hand);
        }

        struct {
            C3D_FVec camera_position;
            C3D_FVec camera_target;
        } t;

        // Matrices
        C3D_Mtx view_matrix;
        C3D_Mtx projection_matrix;

        // Model shader
        DVLB_s *vshader_dvlb;
        shaderProgram_s shader;
        s8 uLoc_projection, uLoc_modelView;

        // Lighting
        C3D_LightEnv light_env;
        C3D_Light light;
        C3D_LightLut lut_phong;
        C3D_LightLut lut_fresnel;
    };

};

#endif //INC_SPLATOON_3DS_GFX_H
