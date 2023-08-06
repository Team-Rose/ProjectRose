#include "rrpch.h"
#include "SceneImporter.h"

#include "RoseRoot/Scene/SceneSerializer.h";

#include "../AssetManagerHolder.h"
namespace Rose {
	namespace Import {
		Ref<Rose::Scene> Scene(Rose::AssetId id, const Rose::AssetMetadata& metadata)
		{
			// This doesn't need to make a copy but I am lazy
			std::filesystem::path path = AssetManagerHolder::GetActiveHolder()->GetAssetDirectory() / metadata.FilePath;
			return Import::Scene(path);
		}
		Ref<Rose::Scene> Import::Scene(const std::filesystem::path& path)
		{
			RR_PROFILE_FUNCTION();

			Ref<Rose::Scene> scene = Rose::CreateRef<Rose::Scene>();
			SceneSerializer serializer(scene);
			serializer.Deserialize(path.string());
			return scene;
		}
	}
	void Save::Scene(Ref<Rose::Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		std::filesystem::path realPath = AssetManagerHolder::GetActiveHolder()->GetAssetDirectory() / path;
		serializer.Serialize(realPath.string());
	}
}


