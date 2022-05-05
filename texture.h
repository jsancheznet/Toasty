#pragma once

#include "typedefs.h"

enum texture_type
{
    TextureType_Null,
    TextureType_Diffuse,
    TextureType_Specular,
};

// NOTE: Maybe i should drop every member in the texture struct and
// just use the GLuint that represents it. It might no be worth it to
// store all these things.
struct texture
{
    texture_type Type;

    u32 Handle;
    i32 Width;
    i32 Height;
    i32 ChannelCount;
    GLenum InternalFormat;
    GLenum Format;
};
