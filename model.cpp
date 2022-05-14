#pragma once

#include "model.h"

void ProcessMesh(const aiMesh *CurrentMesh, std::vector<mesh> &Meshes)
{
    mesh Result = {};
    Result.Name = CurrentMesh->mName.C_Str();

    if(!CurrentMesh->HasPositions()) { printf("Mesh %s: has no positions\n", Result.Name.c_str()); }
    if(!CurrentMesh->HasNormals()) { printf("Mesh %s: has no normals\n", Result.Name.c_str()); }
    if(!CurrentMesh->HasTextureCoords(0)) { printf("Mesh %s: has no texture coordinates\n", Result.Name.c_str()); }

    for(unsigned int j = 0; j < CurrentMesh->mNumVertices; j++)
    {
        vertex Attributes;
        Attributes.Positions          = glm::vec3(CurrentMesh->mVertices[j].x, CurrentMesh->mVertices[j].y, CurrentMesh->mVertices[j].z); // Vertices
        Attributes.Normals            = glm::vec3(CurrentMesh->mNormals[j].x, CurrentMesh->mNormals[j].y, CurrentMesh->mNormals[j].z); // Normals
        Attributes.TextureCoordinates = glm::vec2(CurrentMesh->mTextureCoords[0][j].x, CurrentMesh->mTextureCoords[0][j].y); // TextureCoordinates
        Result.Attributes.push_back(Attributes);
    }

    // Extract Faces/Indices
    if(CurrentMesh->HasFaces())
    {
        for(unsigned int j = 0; j < CurrentMesh->mNumFaces; j++)
        {
            aiFace Face = CurrentMesh->mFaces[j];
            for(unsigned int k = 0; k < Face.mNumIndices; k++)
            {
                Result.Indices.push_back(Face.mIndices[k]);
            }
        }
    }

    // aiMaterial* Material = Scene->mMaterials[Mesh->mMaterialIndex];

    Meshes.push_back(Result);
}

void ProcessNode(const aiScene *Scene, model *Model, const aiNode *Node)
{
    // Set the name for the model or mesh
    if(Node->mParent == NULL)
    {
        Model->Name = Node->mName.C_Str();
    }
    else
    {
        aiString Name = Node->mName;
    }

    // Extract meshes
    int MeshCount = Node->mNumMeshes;
    for(int i = 0; i < MeshCount; i++)
    {
        // Node->mMeshes is an array that contains indices to the Scene->mMeshes array
        ProcessMesh(Scene->mMeshes[Node->mMeshes[i]], Model->Meshes);
    }

    int ChildrenCount = Node->mNumChildren;
    for(int i = 0; i < ChildrenCount; i++)
    {
        ProcessNode(Scene, Model, Node->mChildren[i]);
    }
}

b32 CreateModel(const char *Filename, model *Model)
{
    Assert(Filename);

    model Result = {};

    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(Filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals);
    if (Scene == nullptr)
    {
        printf("%s\n", Importer.GetErrorString());
        return false;
    }

    // Let's process the scene, one node at a time
    ProcessNode(Scene, Model, Scene->mRootNode);

    // TODO: Loop over the materials and store them
    // CONTINUE HERE: https://assimp-docs.readthedocs.io/en/latest/API/API-Documentation.html#_CPPv410aiMaterial
    if(Scene->HasMaterials())
    {
        aiMaterial *Material = Scene->mMaterials[1];

        aiString MaterialName;
        Material->Get(AI_MATKEY_NAME, MaterialName);
        printf("Material Name: %s\n", MaterialName.C_Str());

        aiString Str;
        Material->GetTexture(aiTextureType_DIFFUSE, 0, &Str);
        printf("Texture Name: %s\n", Str.C_Str());

        // Ambient Color
        aiColor3D AmbientColor;
        Material->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor);
        printf("Ambient Color: %2.2f\t%2.2f\t%2.2f\n", AmbientColor.r, AmbientColor.g, AmbientColor.b);

        // Diffuse Color
        aiColor3D DiffuseColor;
        Material->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor);
        printf("Diffuse Color: %2.2f\t%2.2f\t%2.2f\n", DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);

        // Specular
        aiColor3D SpecularColor;
        Material->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor);
        printf("Specular Color: %2.2f\t%2.2f\t%2.2f\n", SpecularColor.r, SpecularColor.g, SpecularColor.b);

        int IlluminationModel;
        Material->Get(AI_MATKEY_SHADING_MODEL, IlluminationModel);
        printf("Illumination Model: %d\n", IlluminationModel);

        float SpecularExponent;
        Material->Get(AI_MATKEY_SHININESS, SpecularExponent);
        printf("Specular Exponent: %2.2f\n", SpecularExponent);
    }

    return true;
}
