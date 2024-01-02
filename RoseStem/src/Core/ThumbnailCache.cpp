#include "ThumbnailCache.h"

namespace Rose {
	ThumbnailCache::ThumbnailCache(Ref<Project> project)
		: m_Project(project)
	{
		m_ThumbnailCachePath =  m_Project->GetCacheDirectory() / "Thumbnail.cache";
	}
	ThumbnailCache::~ThumbnailCache()
	{
	}
	Ref<Texture2D> ThumbnailCache::GetThumbnail(const std::filesystem::path& path)
	{
		return Ref<Texture2D>();
	}
}