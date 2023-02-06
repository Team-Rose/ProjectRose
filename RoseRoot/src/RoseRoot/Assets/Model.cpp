#include "rrpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <RoseRoot/Renderer/Mesh.h>
#include "RoseRoot/Scene/Entity.h"
#include <future>
#include <RoseRoot/Core/Timer.h>
namespace Rose {
	Model::Model()
	{
	}
	bool Model::ReadFile(const std::filesystem::path& path)
	{
		RR_PROFILE_FUNCTION();
		float readFileTime, totalImportTime;
		Timer readFileTimer, totalImportTimer;

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate);
		readFileTime = readFileTimer.ElapsedMillis();

		if (scene == nullptr) {
			RR_CORE_ERROR("Failed to import mesh!: {}", importer.GetErrorString());
			return false;
		}

		RR_CORE_TRACE("Importing external scene file ({}) '{}'", path.string(), scene->mName.C_Str());

		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[i];

			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			
			for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;
				vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertex.TexCoord = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
				//RR_CORE_INFO(vertex.TexCoord);
				vertices.push_back(vertex);
			}

			for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
				const aiFace& Face = mesh->mFaces[i];
				RR_CORE_ASSERT("Face must have 3 indices!", Face.mNumIndices == 3);
				indices.push_back(Face.mIndices[0]);
				indices.push_back(Face.mIndices[1]);
				indices.push_back(Face.mIndices[2]);
			}
			Ref<Mesh> rmesh = CreateRef<Mesh>(vertices, indices);
			m_Meshes.push_back(rmesh);
		}

		totalImportTime = totalImportTimer.ElapsedMillis();

		RR_CORE_TRACE("Imported: {} Mesh(es), Reading the file took: {}ms, Full import took {}ms", scene->mNumMeshes, readFileTime, totalImportTime);
		return true;
	}
}