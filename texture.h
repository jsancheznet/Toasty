#pragma once

#include "typedefs.h"

#include <string>

enum texture_type
{
    // NOTE: Different texture types supported by assimp
    // http://assimp.sourceforge.net/lib_html/material_8h.html#a7dd415ff703a2cc53d1c22ddbbd7dde0
    TextureType_Null,
    TextureType_Diffuse,
    TextureType_Specular,
    TextureType_Ambient,
    TextureType_Emissive,
};

// NOTE: Maybe i should drop every member in the texture struct and
// just use the GLuint that represents it. It might no be worth it to
// store all these things.
struct texture
{
    texture_type Type;
    std::string Filename;

    u32 Handle;
    i32 Width;
    i32 Height;
    i32 ChannelCount;
    GLenum InternalFormat;
    GLenum Format;
};
