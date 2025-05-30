#pragma once

#ifdef _WIN32

	#define PLATFORM_WINDOWS // for some reasone vs 2022 wont use predefined in this file

#endif // WIN32


#ifdef PLATFORM_WINDOWS

#include "App.h"

extern Editor::Application* Editor::CreateApplication(int argc, char** argv);

namespace Editor {

	int Main(int argc, char** argv)
	{
		Editor::Application* app = Editor::CreateApplication(argc, argv);
		app->Run();
		delete app;
		return 0;
	}
}

#ifdef DIST

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Editor::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Editor::Main(argc, argv);
}

#endif // DIST

#endif // PLATFORM_WINDOWS