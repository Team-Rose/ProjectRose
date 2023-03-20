project "RoseStem"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/RoseRoot/vendor/spdlog/include",
		"%{wks.location}/RoseRoot/vendor/lua/sol",
		"%{wks.location}/RoseRoot/src",
		"%{wks.location}/RoseRoot/vendor",
		"%{IncludeDir.filewatch}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Lua}",

		"%{IncludeDir.yaml_cpp}",
	}

	links
	{
		"RoseRoot",
		"yaml-cpp"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "RR_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RR_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RR_DIST"
		runtime "Release"
		optimize "on"
