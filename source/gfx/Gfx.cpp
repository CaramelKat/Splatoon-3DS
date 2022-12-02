//
// Created by ash on 9/16/22.
//

#include "Gfx.h"
#include "vshader_shbin.h"

#define TOON 0
#define FRESNEL 0

#if FRESNEL
// might be a rearranged version of equation 7.3?
// https://developer.download.nvidia.com/CgTutorial/cg_tutorial_chapter07.html
static constexpr float badFresnel(float input, float expo) {
    return fmax(0.0f, 0.9f - powf(fmax(input, 0.0f), expo));
}
#endif

#if TOON
#define NUM_DIFFUSE_TONES 3
static float toon_diffuse(float x, float arg)
{
    const float factor = NUM_DIFFUSE_TONES-1;
    return floorf(0.5f+x*factor)/factor;
}

#define NUM_SPECULAR_TONES 2
static float toon_specular(float x, float shininess)
{
    const float factor = NUM_SPECULAR_TONES-1;
    return floorf(0.5f+powf(x, shininess)*factor)/factor;
}
#endif

Gfx::State::State() {
    // Load the vertex shader, create a shader program and bind it
    vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
    shaderProgramInit(&shader);
    shaderProgramSetVsh(&shader, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&shader);

    // Get the location of the uniforms
    uLoc_projection   = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");
    uLoc_modelView    = shaderInstanceGetUniformLocation(shader.vertexShader, "modelView");

    // Compute the projection matrix
    Mtx_PerspTilt(&projection_matrix, C3D_AngleFromDegrees(60.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, left_hand);

    //fresnel stuff
#if FRESNEL
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_ADD);
    C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
    env = C3D_GetTexEnv(1);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PREVIOUS, GPU_PREVIOUS);
    C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_ONE_MINUS_SRC_ALPHA);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_INTERPOLATE);
    C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
#else
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_FRAGMENT_PRIMARY_COLOR, GPU_FRAGMENT_SECONDARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_ADD);
    env = C3D_GetTexEnv(1);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PREVIOUS);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
#endif
    //TODO: modulating with null texture darkens the shape
    /*env = C3D_GetTexEnv(2);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PREVIOUS, GPU_TEXTURE0);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);*/

    // This should probably move to each Model if it's not costly
    static const C3D_Material material = {
        { 0.2f, 0.2f, 0.2f }, //ambient
        { 0.4f, 0.4f, 0.4f }, //diffuse
        { 0.4f, 0.4f, 0.4f }, //specular0
        { 0.0f, 0.0f, 0.0f }, //specular1
        { 0.0f, 0.0f, 0.0f }, //emission
    };

    // https://github.com/SVatG/HotelNordlicht/blob/master/source/EffectTunnel.c#L479
    // does this count as a 3DS demo for licensing purposes?
    C3D_LightEnvInit(&light_env);
    C3D_LightEnvBind(&light_env);
    C3D_LightEnvMaterial(&light_env, &material);

#if TOON
    /*LightLut_FromFunc(&lut_phong, toon_diffuse, 0.0f, false);
    C3D_LightEnvLut(&light_env, GPU_LUT_D1, GPU_LUTINPUT_LN, false, &lut_phong);

    LightLut_FromFunc(&lut_toon, toon_specular, 30.0f, false);
    C3D_LightEnvLut(&light_env, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &lut_toon);*/
#else
    LightLut_Phong(&lut_phong, 30);
    C3D_LightEnvLut(&light_env, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &lut_phong);
#endif

#if FRESNEL
    LightLut_FromFunc(&lut_fresnel, badFresnel, 1.2, false);
    C3D_LightEnvLut(&light_env, GPU_LUT_FR, GPU_LUTINPUT_NV, false, &lut_fresnel);
    C3D_LightEnvFresnel(&light_env, GPU_PRI_SEC_ALPHA_FRESNEL);
#endif

    C3D_LightInit(&light, &light_env);
    C3D_LightColor(&light, 1.0f, 1.0f, 1.0f);


    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
    C3D_CullFace(GPU_CULL_BACK_CCW);
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);
}

Gfx::State::~State() {
    // Free the shader program
    shaderProgramFree(&shader);
    DVLB_Free(vshader_dvlb);
}
