#include <3ds.h>
#include <3ds/svc.h>
#include <citro3d.h>
#include <cstring>
#include <cmath>
#include <cstdio>
#include "gfx/Model.h"
#include "gfx/Gfx.h"
#include "engine/Map.h"
#include "util.h"
#include "gyro.h"
#include "engine/entities/EntPlayer.h"

#define CLEAR_COLOR 0x68B0D8FF
#define CIRCLE_PAD_DEAD_ZONE 10
#define CIRCLE_PAD_MAX 155.0
#define C_STICK_MAX 155.0 //Why is this different Nintendo???

#define DISPLAY_TRANSFER_FLAGS \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
    GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

static void sceneRender(Gfx::State& gfx, Map& m)
{
    gfx.Update();

    // Update the uniforms
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, gfx.uLoc_projection, &gfx.projection_matrix);

    // Draw the VBO
    m.Draw(gfx);
}

int main()
{
    // Init romfs
    romfsInit();

    // Initialize graphics
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    LOGF("hello world %d\n", 1);
    // Initialize the render target
    C3D_RenderTarget* target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    // Initialise input
    gyro_init();

    // Initialize the scene
    Gfx::State gfx;
    gfx.t.camera_position = FVec3_New(0.0f, 2.0f, 4.0f);
    gfx.t.camera_target = FVec3_New(0.0f, 0.0f, 0.0f);
    gfx.t.light_pos = FVec4_New(0.0f, 3.0f, 0.0f, 1.0f);

    Map level("romfs:/flounder-lows.3map");
    if (!level.valid) {
        LOG("level load failed!");
    }

    auto player = level.AddDynamicEntity<EntPlayer>("miku");

    float camera_distance = 2.0f;

    // Main loop
    while (aptMainLoop())
    {
        hidScanInput();

        // Respond to user input
        u32 kDown = hidKeysDown();
        u32 kHeld = hidKeysHeld();
		circlePosition leftCirclePad, rightCirclePad;
		// Range is -155.0 -> 155.0; * 10 to bring the scale down
		float circlePadScale = CIRCLE_PAD_MAX * 10;
		// Range is -146.0 -> 146; We're adjusting the speed of the camera,
		// Default scale is 32, so we want to ramp up to that point, hence 
		// subtracting down from 64. Still playing with these so they feel good 
		float cStickScale = 80.0 - abs(rightCirclePad.dx / C_STICK_MAX * 35);
		float cStickDirection = rightCirclePad.dx < 0 ? 1 : -1;

		hidCircleRead(&leftCirclePad);
		irrstCstickRead(&rightCirclePad);

        if (kDown & KEY_START)
            break; // break in order to return to hbmenu

		if (abs(leftCirclePad.dy) > CIRCLE_PAD_DEAD_ZONE || abs(leftCirclePad.dx) > CIRCLE_PAD_DEAD_ZONE)
        	player->move(leftCirclePad.dy / circlePadScale, -leftCirclePad.dx / circlePadScale);

		
		if (cStickScale < 80)
			player->rotate({0.0, M_PI * cStickDirection / cStickScale, 0.0});

        if (kDown & KEY_Y)
            gyro_setHome();

    	if (kDown & KEY_DUP)
            camera_distance += 0.5;

        if (kDown & KEY_DDOWN)
            camera_distance -= 0.5;

        // Update camera position (TODO gyro)
        const float sensitivity = 0.05;

        gyro_update();
        printf("\x1b[26;1Hgyro: (%0.2f, %0.2f)\n",
               roll, pitch
        );

        auto pos = player->head_position();
        auto rot = player->rotation();
        gfx.t.camera_target = C3D_FVec(pos);
        // todo move this to vector.h
        gfx.t.camera_position = C3D_FVec {
                .w = 0.0,
                .z = pos.z + cos(roll * sensitivity * -1) * -camera_distance,
                .y = pos.y + camera_distance * (pitch * sensitivity),
                .x = pos.x + sin(roll * sensitivity * -1) * -camera_distance,
        };

        printf("\x1b[26;1Hplayer: (%0.2f, %0.2f, %0.2f)\n"
               "\x1b[27;1Hcamera: (%0.2f, %0.2f, %0.2f)\n",
               pos.x, pos.y, pos.z,
               gfx.t.camera_position.x, gfx.t.camera_position.y, gfx.t.camera_position.z
        );

        printf("\x1b[28;1Hgpu: %5.2f%%  cpu: %5.2f%%  buf:%5.2f%%\n",
               C3D_GetDrawingTime()*3, C3D_GetProcessingTime()*3, C3D_GetCmdBufUsage()*100
        );

		printf("\x1b[29;1HC Stick: %0.2f, %0.2f     \n",
               cStickScale, 32.0 - abs(rightCirclePad.dy / C_STICK_MAX * 10)
        );

        // Render the scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            C3D_RenderTargetClear(target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
            C3D_FrameDrawOn(target);
            sceneRender(gfx, level);
        C3D_FrameEnd(0);
    }

    // Deinitialize graphics
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
