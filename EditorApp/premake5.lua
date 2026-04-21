project "EditorApp"
	language "C++"
	cppdialect "C++17"
	targetdir "bin/%{cfg.buildcfg}"
	staticruntime "off"


	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

	postbuildcommands {
		--'{COPYDIR} "%{wks.location}/EditorApp/imgui.ini" "%{cfg.targetdir}/imgui.ini"',
		'{COPYDIR} "%{wks.location}/EditorApp/assets" "%{cfg.targetdir}/assets"',
		'{COPYDIR} "%{wks.location}/EditorApp/AppAssets" "%{cfg.targetdir}/AppAssets"',
	}

	files
	{
		"src/**.h",
		"src/**.cpp",
		"EditorApp.aps",
		"EditorApp.rc",
		"resource.h",
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
		kind "ConsoleApp"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"
		kind "ConsoleApp"

	filter "configurations:Dist"
		defines "DIST"
		runtime "Release"
		optimize "on"
		kind "WindowedApp"
