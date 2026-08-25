#include "assets_loader.h"
#include "glad.h"
#include "core/decoders/stb_image.h"
#include "core/utils/file_system.h"

bool asset_loader_create_texture_from_pixels(u8* pixels, u32 width, u32 height, u32* out_tex_id)
{
    u32 tex;

    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glGenerateMipmap(GL_TEXTURE_2D);

    if (out_tex_id != NULL) *out_tex_id = tex; // Error otherwise ?
    return tex != 0;
}

bool asset_loader_create_texture_from_file(const char* image_path, u32* out_tex_id)
{
    s32 width, height, channels; 
    stbi_set_flip_vertically_on_load(true); // should be passed as argument ?
    u8* pixels = stbi_load(image_path, &width, &height, &channels, STBI_rgb_alpha);

    UNUSED(width); UNUSED(height); UNUSED(channels);

    if (!pixels)
    {
        LOG_ERROR("Failed loading image: '%s'\n", image_path);
        return false;
    }

    bool result = asset_loader_create_texture_from_pixels(pixels, width, height, out_tex_id);

    stbi_image_free(pixels);

    return result;
}

bool asset_loader_create_shader_from_source(const char* vs, const char* fs, u32* out_shader_id)
{
    u32 prog;
    prog = glCreateProgram();

    u32 v_id = glCreateShader(GL_VERTEX_SHADER);
    u32 f_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(v_id, 1, (const char* const*)&vs, NULL );
    glShaderSource(f_id, 1, (const char* const*)&fs, NULL );

    int success;
    char buff [1024];

    glCompileShader(v_id);
    glGetShaderiv(v_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(v_id, 1024, NULL, buff);
        LOG_ERROR("'%s': vertex shader error: \n%s\n", vs, buff);
        return false;
    }

    glCompileShader(f_id);
    glGetShaderiv(f_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(f_id, 1024, NULL, buff);
        LOG_ERROR("'%s': fragmnet shader error: \n%s\n", fs, buff);
        return false;
    }

    glAttachShader(prog, v_id);
    glAttachShader(prog, f_id);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(prog, 1024, NULL, buff);
        LOG_ERROR("'%s', '%s': Linking shaders error: \n%s\n", vs, fs, buff);
        return false;
    }

    glDeleteShader(v_id);
    glDeleteShader(f_id);

    if (out_shader_id != NULL) *out_shader_id = prog; // Error otherwise ?
    return true;
}

bool asset_loader_create_shader_from_file(const char* vs_path, const char* fs_path, u32* out_shader_id)
{
    char* vs = read_entire_file(vs_path);
    char* fs = read_entire_file(fs_path);

    bool res = asset_loader_create_shader_from_source(vs, fs, out_shader_id);

    free(vs);
    free(fs);

    return res;
}