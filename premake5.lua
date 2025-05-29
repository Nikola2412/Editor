workspace "App"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "App"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "vendor/imgui"
   include "vendor/glfw"
group ""

group "Core"
include "Editor"
group ""

include "App"