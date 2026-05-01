#pragma once

namespace Editor
{
	enum FileDialogFlags
	{
		PNG = 1 << 0,
		JPG = 1 << 1,
	};

	class FileDialog
	{
		public:
		static bool OpenFile(const char* filter, std::string& outPath);
		static bool OpenFile(int flags, std::string& outPath);
		static bool SaveFile(const char* filter, std::string& outPath);
	};

	class WindowsFileDialog
	{
		public:
		static bool OpenFile(const char* filter, std::string& outPath);
		static bool OpenFile(int flags, std::string& outPath);
		static bool SaveFile(const char* filter, std::string& outPath);
	};

}