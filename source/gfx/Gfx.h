//
// Created by ash on 9/16/22.
//

#ifndef INC_SPLATOON_3DS_GFX_H
#define INC_SPLATOON_3DS_GFX_H

#include <citro3d.h>

#define TEXTURE_TEXENV 2

namespace Gfx {
    const bool left_hand = false;
    const C3D_FVec Up = FVec3_New(0.0f, 1.0f, 0.0f);

    class State {
    public:
        State();
        ~State();
        void Update() {
            Mtx_LookAt(&view_matrix, t.camera_position, t.camera_target, Up, left_hand);

            C3D_FVec light_vec = Mtx_MultiplyFVec4(&view_matrix, t.light_pos);
            C3D_LightPosition(&light, &light_vec);
        }

        struct {
            C3D_FVec camera_position;
            C3D_FVec camera_target;

            C3D_FVec light_pos;
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
        C3D_LightLut lut_toon;
    };

};

#endif //INC_SPLATOON_3DS_GFX_H
