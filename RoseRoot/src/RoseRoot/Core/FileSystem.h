#pragma once
#include "RoseRoot/Core/Buffer.h"

namespace Rose {

	// TODO: platforms
	class FileSystem
	{
	public:
		// TODO: Move to FileSystem class
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};
}