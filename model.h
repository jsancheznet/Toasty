#pragma once

// #include <iostream> // including iostream breaks the build somehow, what is going on?
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "typedefs.h"

struct vertex
{
    glm::vec3 Positions;
    glm::vec3 Normals;
    glm::vec2 TextureCoordinates;
};

struct mesh
{
    std::string Name;
    std::vector<vertex> Attributes;
    std::vector<u32> Indices;
};

struct model
{
    std::string Name;
    std::vector<mesh> Meshes;

    // Materiales
    // std::string Directory;
    // std::vector<texture> Textures;
};
