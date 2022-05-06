include "Dependencies.lua"

workspace "Rose"
	architecture "x86_64"
	startproject "RoseStem"

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

group "Dependencies"
	include "RoseRoot/vendor/Box2D"
	include "RoseRoot/vendor/GLFW"
	include "RoseRoot/vendor/Glad"
	include "RoseRoot/vendor/imgui"
	include "RoseRoot/vendor/yaml-cpp"
group ""

include "RoseRoot"
include "Sandbox"
include "RoseStem"
