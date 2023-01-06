#include "Project.h"

#include "ProjectSerializer.h"
namespace Rose
{
	Project::Project()
	{
	}
	Project::~Project()
	{
	}
	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		s_ActiveProject->GetConfig().AssetDirectory = "assets";

		return s_ActiveProject;
	}
	Ref<Project> Project::Load(const std::filesystem::path& path) {
		Ref <Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			s_ActiveProject = project;
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return s_ActiveProject;
		}
		
		return nullptr;
	}
	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}
		return false;
	}
}