#pragma once
#include "AssetManagers/AssetManagerBase.h"
#include "AssetManagers/EditorAssetManager.h"

// Acts as a interface between the apps project (or similar concept) and RoseRoot for assets
namespace Rose {
	class AssetManagerHolder
	{
	public:
		virtual Ref<AssetManagerBase> GetAssetManager() = 0;
#ifdef RR_ASSET_MANAGER_BUILD_EDITOR
		Ref<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(GetAssetManager()); }

		virtual const std::filesystem::path GetAssetDirectory() = 0;
#endif // RR_ASSET_MANAGER_BUILD_EDITOR

		static Ref<AssetManagerHolder> GetActiveHolder() {
			return s_ActiveHolder;
		}
		static void SetActiveHolder(Ref<AssetManagerHolder> newHolder) {
			s_ActiveHolder = newHolder;
		}
	private:
		inline static Ref<AssetManagerHolder> s_ActiveHolder = nullptr;
	};
}