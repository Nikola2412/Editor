#include "pch.h"
#include "Log.h"

namespace Editor
{
	Editor::Scope<AsyncLogger> Log::coreLogger;
	Editor::Scope<AsyncLogger> Log::clientLogger;

	void Log::Init()
	{
		coreLogger = CreateScope<AsyncConsoleLogger>();
		clientLogger = CreateScope<AsyncFileLogger>();
	}
}