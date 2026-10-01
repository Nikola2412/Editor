# Editor

A lightweight desktop application builder built in C++20, using GLFW and Dear ImGui for the UI layer, backed by a small custom engine designed for simplicity, performance, and extensibility.

## Features

- Browse a sequence of images in a desktop UI
- Previous/next navigation controls
- Slide and morph animation effects
- Adjustable image size and rotation
- Save and open PNG assets from the app
- VSync toggle and frame timing information
- Cross-platform-friendly engine foundation with a Windows-focused runtime

## Tech Stack

- C++20
- GLFW
- Glad
- Dear ImGui
- GLM
- stb_image
- Custom logging utilities

## Prerequisites

Before building the project, make sure you have:

- Windows 10 or 11
- Visual Studio 2022/26
- Desktop development with C++ workload installed
- Git with submodules support enabled

## Quick Start

Clone the repository with submodules:

```bash
git clone --recurse-submodules https://github.com/Nikola2412/Editor.git
cd Editor
```

Generate the Visual Studio solution:

```bat
script\sln gen.bat
```
or
```bat
script\slnx gen.bat
```

This uses the bundled Premake toolchain to generate the project files for Visual Studio 2022/26.

Then open the generated solution in Visual Studio, set `EditorApp` as the startup project, and build the solution.

## Project Structure

```text
Editor/
├── Editor/                 # Core engine and shared code
├── EditorApp/              # Desktop application entry point and UI
├── premake/                # Premake binaries used to generate project files
├── script/                 # Helper scripts for generating solutions
├── README.md               # Project overview
├── premake5.lua            # Workspace configuration
├── Editor.slnx             # Solution file generated for the workspace
└── LICENSE                 # Project license
```

## Build Notes

- The workspace is configured with Premake and uses the `EditorApp` startup project.
- Generated binaries are placed under the `bin/` folder.
- Runtime assets for the app are copied into the output directory during the build.

## License

This project is distributed under the terms of the included license. See [LICENSE](LICENSE) for details.

