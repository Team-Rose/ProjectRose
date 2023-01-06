#pragma once
#include "RoseRoot.h"

#include "Project.h"

namespace Rose {
	class ProjectSerializer {
	public:
		ProjectSerializer(Ref<Project> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		bool DeserializeV1(const std::filesystem::path& filepath);

		Ref<Project> m_Project;
	};
}