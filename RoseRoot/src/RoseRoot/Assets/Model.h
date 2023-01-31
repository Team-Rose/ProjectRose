#pragma once
#include <RoseRoot/Scene/Scene.h>
#include "RoseRoot/Renderer/Mesh.h"
//TODO (Sam) Consider renaming this sense it is used for storage and import
namespace Rose {
	class Model {
	public:
		Model();
		~Model() = default;

		bool ReadFile(const std::filesystem::path& path);

		inline const std::vector<Ref<Mesh>>& GetMeshes() { return m_Meshes; }
	private:
		std::vector<Ref<Mesh>> m_Meshes;
	};
}