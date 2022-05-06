#pragma once

#include <string>

namespace Rose
 {

	class FileDialogs
	{
	public:
		static std::string OpenDirectory(const char* filter);

		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}