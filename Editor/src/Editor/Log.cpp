#include "pch.h"
#include "Log.h"

namespace Editor
{
	Editor::Ref<AsyncLogger> Log::coreLogger;
	Editor::Ref<AsyncLogger> Log::clientLogger;

	void Log::Init()
	{
		coreLogger = CreateRef<AsyncConsoleLogger>("Editor");
#ifdef DEBUG
		std::vector<Ref<AsyncLogger>> v;
		v.emplace_back(CreateRef<AsyncFileLogger>());
		v.emplace_back(CreateRef<AsyncConsoleLogger>());
		clientLogger = CreateRef<AsyncLogger>(v, "App");
#else
		clientLogger = CreateRef<AsyncConsoleLogger>("App");
#endif // DEBUG
		
	}
}