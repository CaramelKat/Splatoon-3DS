#include <3ds.h>
#include <3ds/svc.h>
#include <citro3d.h>
#include <cstring>
#include <cmath>
#include <cstdio>
#include "gfx/Model.h"
#include "gfx/Gfx.h"
#include "util.h"

#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

static void sceneRender(Gfx::State& gfx, Model& m)
{
    gfx.Update();

    /*C3D_FVec lightPos = FVec4_New(0.0f, 0.0f, -0.5f, 1.0f);
    C3D_LightPosition(&gfx.light, &lightPos);*/

	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, gfx.uLoc_projection, &gfx.projection_matrix);

	// Draw the VBO
    m.Draw(gfx);
}

int main()
{
	// Initialize graphics
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    LOGF("hello world %d\n", 1);
	// Initialize the render target
	C3D_RenderTarget* target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Initialize the scene
	Gfx::State gfx;
    gfx.t.camera_position = FVec3_New(0.0f, 5.0f, -0.5f);

    Model donut("donut.3mdl");
    donut.world_position = FVec3_New(0.0f, 0.0f, 2.0f);
    gfx.t.camera_target = donut.world_position;
	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

        if (kDown & KEY_UP)
            gfx.t.camera_position.y += 0.5f;

        if (kDown & KEY_DOWN)
            gfx.t.camera_position.y -= 0.5f;

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C3D_RenderTargetClear(target, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
			C3D_FrameDrawOn(target);
			sceneRender(gfx, donut);
		C3D_FrameEnd(0);
	}

	// Deinitialize graphics
	C3D_Fini();
	gfxExit();
	return 0;
}