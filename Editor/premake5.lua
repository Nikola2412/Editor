project "Editor"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
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
        "vendor/glm/glm/**.hpp",
 	    "vendor/glm/glm/**.inl",
   }

   includedirs
   {
        "src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}"
   }

   defines
   {
 		"_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
   }

   links
   {
        "GLFW",
        "Glad",
        "ImGui"
   }

   buildoptions { "/utf-8" }


   filter "system:windows"
      systemversion "latest"
      defines { "PLATFORM_WINDOWS" }

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