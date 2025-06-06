#pragma once


#ifdef PLATFORM_WINDOWS

#include "App.h"

namespace Editor {

	int Main(int argc, char** argv)
	{
		auto app = Editor::CreateApplication();
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