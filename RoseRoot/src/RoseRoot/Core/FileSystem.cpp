#include "rrpch.h"
#include "FileSystem.h"

#include <fstream>

namespace Rose {
	Buffer FileSystem::ReadFileBinary(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);
		if (!stream)
		{
			// Failed to open the file
			return Buffer();
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return Buffer();
		}

		Buffer buffer(size);
		stream.read(buffer.As<char>(), size);
		stream.close();

		return buffer;
	}
}