workspace "Editor"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "EditorApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glm"] = "../vendor/glm"
IncludeDir["Glad"] = "vendor/Glad/include"


group "Dependencies"
   include "vendor/imgui"
   include "vendor/glfw"
   include "Editor/vendor/Glad"
group ""

group "Editor"
include "Editor"
group ""

include "EditorApp"