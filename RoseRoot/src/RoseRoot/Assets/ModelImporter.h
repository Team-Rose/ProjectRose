#pragma once
#include <RoseRoot/Scene/Scene.h>

namespace Rose {
	class ModelImporter {
	public:
		ModelImporter();
		~ModelImporter() = default;

		bool ReadFile(const std::filesystem::path& path);

	private:
		Ref<Scene> m_Scene = nullptr;
	};
}