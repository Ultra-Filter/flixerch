#pragma once

#include "core/utils/defines.h"

// typedef struct texture2D_s {
//     u32 openGL_texID;
//     u32 assetID;
//     u32 height, width;
//     const char * image_path;
// } texture2D_s;

bool asset_loader_create_shader_from_file(const char* vs_path, const char* fs_path, u32* out_shader_id);
bool asset_loader_create_shader_from_source(const char* vs_src, const char* fs_src, u32* out_shader_id);
bool asset_loader_create_texture_from_pixels(u8* pixels, u32 width, u32 height, u32* out_tex_id);
bool asset_loader_create_texture_from_file(const char* image_path, u32* out_tex_id);

 