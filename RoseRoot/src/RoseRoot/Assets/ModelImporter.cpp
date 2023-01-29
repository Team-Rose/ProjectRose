#include "rrpch.h"
#include "ModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <RoseRoot/Renderer/Mesh.h>
#include "RoseRoot/Scene/Entity.h"
namespace Rose {
	ModelImporter::ModelImporter()
	{
		m_Scene = CreateRef<Scene>();
	}
	bool ModelImporter::ReadFile(const std::filesystem::path& path)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate);

		if (scene == nullptr) {
			RR_CORE_ERROR("Failed to import mesh!: {}", importer.GetErrorString());
			return false;
		}

		RR_CORE_TRACE("Importing external scene file ({}) '{}'", path.string(), scene->mName.C_Str());
		RR_CORE_TRACE("{} Mesh(es)", scene->mNumMeshes);

		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[i];

			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;
				vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertex.TexCoord = glm::vec2(0.0f, 0.0f);
				vertices.push_back(vertex);
			}

			for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
				const aiFace& Face = mesh->mFaces[i];
				RR_CORE_ASSERT("Face must have 3 indices!", Face.mNumIndices == 3);
				indices.push_back(Face.mIndices[0]);
				indices.push_back(Face.mIndices[1]);
				indices.push_back(Face.mIndices[2]);
			}
			Entity& entity = m_Scene->CreateEntity(mesh->mName.C_Str());
			auto& mrc = entity.AddComponent<MeshRendererComponent>();
			//mrc.Path = path;
		}

		return true;
	}
}