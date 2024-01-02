#pragma once

#include "Project/Project.h"
#include "RoseRoot/Renderer/Texture.h"

namespace Rose {
	struct ThumbnailImagew
	{
		uint32_t TimestampHash;
		Ref<Texture2D> Image;
	};

	class  ThumbnailCache
	{
	public:
		ThumbnailCache(Ref<Project> project);
		~ThumbnailCache();

		Ref<Texture2D> ThumbnailCache::GetThumbnail(const std::filesystem::path& path);
	private:
		Ref<Project> m_Project;

//		std::map<std::filesystem::path, ThumbNailImage> m_CachedImages;
		std::filesystem::path m_ThumbnailCachePath;
	};
}