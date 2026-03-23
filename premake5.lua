workspace "Editor"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   flags { "MultiProcessorCompile" }
   startproject "EditorApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["stb_image"] =	"%{wks.location}/Editor/vendor/stb_image"
IncludeDir["glm"]	=		"%{wks.location}/Editor/vendor/glm"
IncludeDir["ImGui"] =		"%{wks.location}/Editor/vendor/Imgui"
IncludeDir["AsyncLogger"] =	"%{wks.location}/Editor/vendor/AsyncLogger/AsyncLogger"
IncludeDir["GLFW"]	=		"%{wks.location}/Editor/vendor/GLFW/include"
IncludeDir["Glad"]	=		"%{wks.location}/Editor/vendor/Glad/include"


group "Dependencies"
   include "Editor/vendor/glfw"
   include "Editor/vendor/Glad"
   include "Editor/vendor/ImGui"
   include "Editor/vendor/AsyncLogger"
group ""

group "Editor"
	include "Editor"
group ""

include "EditorApp"