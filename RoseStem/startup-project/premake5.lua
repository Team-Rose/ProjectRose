-- Implement the solution_items command for solution-scope files
require('vstudio')

premake.api.register {
	name = "solution_items",
	scope = "workspace",
	kind = "list:string",
}

premake.override(premake.vstudio.sln2005, "projects", function(base, wks)
	if wks.solution_items and #wks.solution_items > 0 then
		local solution_folder_GUID = "{2150E333-8FDC-42A3-9474-1A3956D46DE8}" -- See https://www.codeproject.com/Reference/720512/List-of-Visual-Studio-Project-Type-GUIDs
		premake.push("Project(\"" .. solution_folder_GUID .. "\") = \"Solution Items\", \"Solution Items\", \"{" .. os.uuid("Solution Items:" .. wks.name) .. "}\"")
		premake.push("ProjectSection(SolutionItems) = preProject")

		for _, path in ipairs(wks.solution_items) do
			premake.w(path .. " = " .. path)
		end

		premake.pop("EndProjectSection")
		premake.pop("EndProject")
	end
	base(wks)
end)


workspace "Startup-Project"
	architecture "x86_64"
	startproject "Startup-Project"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Startup-Project"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("Binaries")
	objdir ("Intermediates")

	includedirs  {
		"assets"
	}
	
	files
	{
		"assets/**.cs",
		"Properties/**.cs"
	}

	links
	{
		"Rose-ScriptCore"
	}

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "On"
		symbols "Off"

group "Rose"
	--TODO Make this change automagically when making a new project
	include "../../Rose-ScriptCore"
group ""