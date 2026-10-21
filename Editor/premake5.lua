project "Editor"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   pchheader "pch.h"
   pchsource "src/pch.cpp"

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   files 
   {
        "src/**.h",
        "src/**.cpp",
        "vendor/imgui/backends/imgui_impl_glfw.cpp",
        "vendor/imgui/backends/imgui_impl_opengl3.cpp",
        "vendor/glm/glm/**.hpp",
 	    "vendor/glm/glm/**.inl",
   }

   if HasVulkanSDK then
      files { "vendor/imgui/backends/imgui_impl_vulkan.cpp" }
   end

   filter "files:vendor/imgui/backends/*.cpp"
      flags { "NoPCH" }
   filter {}

   includedirs
   {
        "src",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.AsyncLogger}",
   }

   if HasVulkanSDK then
      includedirs { "%{IncludeDir.Vulkan}" }
      libdirs { path.join(VulkanSDK, "Lib") }
      defines { "EDITOR_HAS_VULKAN" }
      links { "vulkan-1" }
   end

   defines
   {
 		"_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
   }

   links
   {
        "ImGui",
        "GLFW",
        "Glad",
        "AsyncLogger"
   }

   filter "system:windows"
      systemversion "latest"
      defines { "PLATFORM_WINDOWS" }
      buildoptions { "/utf-8" }

   filter "system:macosx"
      files { "src/**.mm" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      runtime "Debug"
      symbols "On"

   filter "configurations:Release"
      defines { "RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      defines { "DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"