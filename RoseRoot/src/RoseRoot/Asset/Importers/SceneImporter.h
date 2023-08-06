#pragma once
#include "RoseRoot/Scene/Scene.h"
#include "RoseRoot/Asset/AssetMetadata.h"

namespace Rose {
	namespace Import {
		Ref<Rose::Scene> Scene(Rose::AssetId id, const Rose::AssetMetadata& metadata);
		Ref<Rose::Scene> Scene(const std::filesystem::path& path);
	}
	namespace Save {
		void Scene(Ref<Rose::Scene> scene, const std::filesystem::path& path);
	}
}