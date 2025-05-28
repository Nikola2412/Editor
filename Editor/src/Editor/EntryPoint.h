#pragma once

#define PLATFORM_WINDOWS // for some reasone vs 2022 wont use predefined 

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

#ifdef WL_DIST

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

#endif // WL_DIST

#endif // WL_PLATFORM_WINDOWS