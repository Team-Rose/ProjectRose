#pragma once
#include <RoseRoot/Core/UUID.h>

namespace Rose {
	using AssetID = UUID;
	class Asset {
	public:
		virtual ~Asset() = default;
		

		//virtual Ref<void> GetAssetRef();
	};
}