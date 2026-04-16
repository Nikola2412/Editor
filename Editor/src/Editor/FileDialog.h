#pragma once

namespace Editor
{
	class FileDialog
	{
		public:
		static bool OpenFile(const char* filter, std::string& outPath);
		static bool SaveFile(const char* filter, std::string& outPath);
	};

	class WindowsFileDialog
	{
		public:
		static bool OpenFile(const char* filter, std::string& outPath);
		static bool SaveFile(const char* filter, std::string& outPath);
	};

}