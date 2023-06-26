#pragma once

#include "../Asset.h"
#include "../AssetMetadata.h"

#include "RoseRoot/Renderer/Texture.h"
namespace Rose {
	namespace Import {
		Ref<Rose::Texture2D> Texture2D(Rose::AssetId id, const Rose::AssetMetadata& metadata);
		Ref<Rose::Texture2D> Texture2D(const std::filesystem::path& path);
	}
}