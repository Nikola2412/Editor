#include "pch.h"
#include "FileDialog.h"
#include "App.h"
#include <commdlg.h>


bool Editor::FileDialog::OpenFile(const char* filter, std::string& outPath)
{
#ifdef _WIN32
	return WindowsFileDialog::OpenFile(filter, outPath);
#else
#error "FileDialog is not implemented for this platform"
	return false;

#endif // _WIN32
}

bool Editor::FileDialog::SaveFile(const char* filter, std::string& outPath)
{
#ifdef _WIN32
	return WindowsFileDialog::SaveFile(filter, outPath);
#else
#error "FileDialog is not implemented for this platform"
	return false;

#endif // _WIN32
}
#ifdef _WIN32
#include <Windows.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif // _WIN32

bool Editor::WindowsFileDialog::OpenFile(const char* filter, std::string& outPath)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };
	CHAR currentDir[256] = { 0 };
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	if (GetCurrentDirectoryA(256, currentDir))
		ofn.lpstrInitialDir = currentDir;
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		outPath = ofn.lpstrFile;
		return true;
	}
	return false;
}

bool Editor::WindowsFileDialog::SaveFile(const char* filter, std::string& outPath)
{
	return false;
}
