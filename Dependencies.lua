VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["filewatch"] = "%{wks.location}/RoseRoot/vendor/filewatch"
IncludeDir["stb_image"] = "%{wks.location}/RoseRoot/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/RoseRoot/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/RoseRoot/vendor/Box2D/include"
IncludeDir["GLFW"] = "%{wks.location}/RoseRoot/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/RoseRoot/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/RoseRoot/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/RoseRoot/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/RoseRoot/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/RoseRoot/vendor/entt/include"
IncludeDir["Lua"] = "%{wks.location}/RoseRoot/vendor/lua/include"
IncludeDir["Mono"] = "%{wks.location}/RoseRoot/vendor/mono/include"
IncludeDir["shaderc"] = "%{wks.location}/RoseRoot/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/RoseRoot/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["msdfgen"] = "%{wks.location}/RoseRoot/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/RoseRoot/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["TinyGLTF"] = "%{wks.location}/RoseRoot/vendor/TinyGLTF/include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["Mono"] = "%{wks.location}/RoseRoot/vendor/mono/lib/%{cfg.buildcfg}"
LibraryDir["Lua"] = "%{wks.location}/RoseRoot/vendor/lua"


Library = {}
Library["Lua"] = "%{LibraryDir.Lua}/lua54.lib"
Library["Mono"] = "%{LibraryDir.Mono}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "BCrypt.lib"