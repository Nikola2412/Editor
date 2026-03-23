project "EditorApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir "bin/%{cfg.buildcfg}"
	staticruntime "off"


	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/Editor/src",
		"%{wks.location}/Editor/vendor",
		"%{wks.location}/Editor/vendor/stb_image",
		"%{wks.location}/Editor/vendor/glm",
		"%{wks.location}/Editor/vendor/Imgui",
		"%{wks.location}/Editor/vendor/AsyncLogger/AsyncLogger",
		"%{wks.location}/Editor/vendor/GLFW/include",
		"%{wks.location}/Editor/vendor/Glad/include"
	}

	links
	{
		"Editor"
	}

	buildoptions
	{ 
		"/utf-8" ,
		"/IGNORE:4099"
	}

	filter "system:windows"
      systemversion "latest"
      defines { "PLATFORM_WINDOWS" }


	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "DIST"
		runtime "Release"
		optimize "on"
