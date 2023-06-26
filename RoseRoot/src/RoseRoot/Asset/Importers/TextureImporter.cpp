#include "rrpch.h"
#include "TextureImporter.h"

#include "stb_image.h"
#include "../AssetManagerHolder.h"
namespace Rose {
	Ref<Texture2D> Import::Texture2D(AssetId id, const AssetMetadata& metadata)
	{
		return Import::Texture2D(AssetManagerHolder::GetActiveHolder()->GetAssetDirectory() / metadata.FilePath);
	}
	Ref<Rose::Texture2D> Import::Texture2D(const std::filesystem::path& path)
	{
		RR_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;
		{
			RR_PROFILE_SCOPE("stbi_load - Import::Texture2D");
			data.Data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
		}

		if (!data)
		{
			RR_CORE_ERROR("Import::Texture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}
		Texture2DSpecification spec;
		spec.Width = width;
		spec.Height = height;
		switch (channels)
		{
		case 3:
			spec.Format = ImageFormat::RGB8;
			data.Size = width * height * 3;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			data.Size = width * height * 4;
			break;
		}

		Ref<Rose::Texture2D> texture = Texture2D::Create(spec, data);
		data.Release();
		//stbi_image_free(data);
		return texture;
	}
}


