#include "pch.h"
#include "Log.h"

namespace Editor
{
	Editor::Ref<AsyncLogger> Log::coreLogger;
	Editor::Ref<AsyncLogger> Log::clientLogger;

	void Log::Init()
	{
#ifdef DEBUG
		{
			std::vector<Ref<AsyncLogger>>v1;
			v1.emplace_back(CreateRef<AsyncFileLogger>("Core", "CoreLogger.txt"));
			v1.emplace_back(CreateRef<AsyncConsoleLogger>("Core"));
			coreLogger = CreateRef<AsyncLogger>(v1, "Core");
		}
		{
			std::vector<Ref<AsyncLogger>> v2;
			v2.emplace_back(CreateRef<AsyncFileLogger>("App", "AppLogger.txt"));
			v2.emplace_back(CreateRef<AsyncConsoleLogger>("App"));
			clientLogger = CreateRef<AsyncLogger>(v2, "App");
		}
#else
		coreLogger = CreateRef<AsyncConsoleLogger>("Editor");
		clientLogger = CreateRef<AsyncConsoleLogger>("App");
#endif // DEBUG
		
	}
}