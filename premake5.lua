workspace "Editor"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   flags { "MultiProcessorCompile" }
   startproject "EditorApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glm"]	= "%{wks.location}/Editor/vendor/glm"
IncludeDir["GLFW"]	= "%{wks.location}/Editor/vendor/GLFW/include"
IncludeDir["Glad"]	= "%{wks.location}/Editor/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Editor/vendor/ImGui"


group "Dependencies"
   include "Editor/vendor/glfw"
   include "Editor/vendor/Glad"
   include "Editor/vendor/ImGui"
group ""

group "Editor"
	include "Editor"
group ""

include "EditorApp"