//
// Created by ash on 9/15/22.
//

#include <citro3d.h>
#include "Model.h"
#include "3mdl.h"
#include "util.h"
#include "concrete_t3x.h"
#include <cstdio>
#include <tex3ds.h>

// Helper function for loading a texture from memory
static bool loadTextureFromMem(C3D_Tex* tex, C3D_TexCube* cube, const void* data, size_t size)
{
    Tex3DS_Texture t3x = Tex3DS_TextureImport(data, size, tex, cube, false);
    if (!t3x)
        return false;

    // Delete the t3x object since we don't need it
    Tex3DS_TextureFree(t3x);
    return true;
}

Model::Model(const std::string& path) {
    MDL_Header header {};
    FILE* fd = fopen(path.c_str(), "rb");
    if (!fd) return;
    auto res = fread(&header, sizeof(header), 1, fd);
    if (res != 1 || (header.dtype != MDL_FLAT && header.dtype != MDL_INDEXED_U16)) {
        fclose(fd);
        return;
    }

    if (header.dtype == MDL_INDEXED_U16) {
        indexes = { (index_type*)linearAlloc(header.indexes_sz * sizeof(index_type)), header.indexes_sz };
        fread(indexes.data(), sizeof(indexes[0]), indexes.size(), fd);
    }

    vertexes = { (vertex_type*)linearAlloc(header.vertexes_sz * sizeof(vertex_type)), header.vertexes_sz };
    fread(vertexes.data(), sizeof(vertexes[0]), vertexes.size(), fd);

    fclose(fd);

    if (!loadTextureFromMem(&texture, NULL, concrete_t3x, concrete_t3x_size))
        svcBreak(USERBREAK_PANIC);
    C3D_TexSetFilter(&texture, GPU_NEAREST, GPU_NEAREST);

    valid = true;
}

void Model::Draw(Gfx::State& gfx, vec3 world_position, std::span<colour_type> colours) {
    if (!valid) return;

    C3D_Mtx model_matrix;
    Mtx_Identity(&model_matrix);
    Mtx_Translate(&model_matrix, world_position.x, world_position.y, world_position.z, false);
    //Mtx_RotateX(&model_matrix, angleX, true);
    //Mtx_RotateZ(&model_matrix, angleY, true);
    Mtx_Multiply(&model_matrix, &gfx.view_matrix, &model_matrix);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, gfx.uLoc_modelView, &model_matrix);

    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=uv
    AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal
    if (!colours.empty()) {
        AttrInfo_AddLoader(attrInfo, 3, GPU_FLOAT, 3); // v3=colour
    } else {
        AttrInfo_AddFixed(attrInfo, 3);
        C3D_FixedAttribSet(3, colour.r, colour.g, colour.b, 1.0f);
    }

    // Configure buffers
    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vertexes.data(), sizeof(vertexes[0]), 3, 0x210);
    if (!colours.empty()) {
        BufInfo_Add(bufInfo, colours.data(), sizeof(colours[0]), 1, 0x3);
    }

    // Bind texture
    C3D_TexBind(0, &texture);

    if (indexes.empty()) {
        C3D_DrawArrays(GPU_TRIANGLES, 0, vertexes.size());
    } else {
        C3D_DrawElements(GPU_TRIANGLES, indexes.size(), C3D_UNSIGNED_SHORT, indexes.data());
    }
}

Model::~Model() {
    linearFree(indexes.data());
    indexes = {};

    linearFree(vertexes.data());
    vertexes = {};
}
