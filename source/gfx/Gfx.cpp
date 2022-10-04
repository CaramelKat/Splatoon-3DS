//
// Created by ash on 9/16/22.
//

#include "Gfx.h"
#include "vshader_shbin.h"

Gfx::State::State() {
    // Load the vertex shader, create a shader program and bind it
    vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
    shaderProgramInit(&shader);
    shaderProgramSetVsh(&shader, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&shader);

    // Get the location of the uniforms
    uLoc_projection   = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");
    uLoc_modelView    = shaderInstanceGetUniformLocation(shader.vertexShader, "modelView");
    /*uLoc_lightVec     = shaderInstanceGetUniformLocation(shader.vertexShader, "lightVec");
    uLoc_lightHalfVec = shaderInstanceGetUniformLocation(shader.vertexShader, "lightHalfVec");
    uLoc_lightClr     = shaderInstanceGetUniformLocation(shader.vertexShader, "lightClr");
    uLoc_material     = shaderInstanceGetUniformLocation(shader.vertexShader, "material");*/

    // Compute the projection matrix
    Mtx_PerspTilt(&projection_matrix, C3D_AngleFromDegrees(80.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, left_hand);

    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

    // This should probably move to each Model if it's not costly
    /*static const C3D_Material material = {
        { 0.3f, 0.3f, 0.3f }, //ambient
        { 0.4f, 0.4f, 0.4f }, //diffuse
        { 0.3f, 0.3f, 0.3f }, //specular0
        { 0.0f, 0.0f, 0.0f }, //specular1
        { 0.0f, 0.0f, 0.0f }, //emission
    };

    C3D_LightEnvInit(&light_env);
    C3D_LightEnvBind(&light_env);
    C3D_LightEnvMaterial(&light_env, &material);

    LightLut_Phong(&lut_spec, 20.0f);
    C3D_LightEnvLut(&light_env, GPU_LUT_D0, GPU_LUTINPUT_NH, false, &lut_spec);

    C3D_LightInit(&light, &light_env);*/
}

Gfx::State::~State() {
    // Free the shader program
    shaderProgramFree(&shader);
    DVLB_Free(vshader_dvlb);
}