#pragma once


#ifdef PLATFORM_WINDOWS

#include "App.h"

namespace Editor {

	int Main()
	{
		auto app = Editor::CreateApplication();
		app->Run();
		delete app;
		return 0;
	}
}

#ifdef DIST

#include <Windows.h>

int APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	return Editor::Main();
}


#else

int main(int argc, char** argv)
{
	Editor::Log::Init();

	return Editor::Main();
}

#endif // DIST

#endif // PLATFORM_WINDOWS