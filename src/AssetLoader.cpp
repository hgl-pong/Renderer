#include <AssetLoader.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include "Functions.h"
void ModelLoader::ReadFile(const std::string &filename, Model &model)
{
    auto it = m_models.find(filename);
    if (it != m_models.end())
    {
        model = it->second;
        return;
    }

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    if (!scene)
    {
        printf("Error loading model %s\n", filename.c_str());
        return;
    }
    model.m_meshes.resize(scene->mNumMeshes);
    for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[i];
        auto &newMesh = model.m_meshes[i];
        // newMesh.m_vertices.resize(mesh->mNumVertices);
        newMesh.m_indices.resize(mesh->mNumFaces * 3);
        for (uint32_t j = 0; j < mesh->mNumVertices; ++j)
        {
            aiVector3D vertex = mesh->mVertices[j];
            // newMesh.m_vertices.push_back(physx::PxVec3(vertex.x, vertex.y, vertex.z));
        }
        for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
        {
            aiFace face = mesh->mFaces[j];
            for (uint32_t k = 0; k < face.mNumIndices; ++k)
            {
                newMesh.m_indices.push_back(face.mIndices[k]);
            }
        }
        newMesh.m_color = RandomColor();
    }
    m_models.emplace(std::make_pair(filename, model));
}