#pragma once

#include "Core.h"

#include <AsyncLogger.h>

namespace Editor
{
	class Log
	{
	public:
		static void Init();
		inline static Scope<AsyncLogger>& GetCoreLogger() { return coreLogger; }
		inline static Scope<AsyncLogger>& GetClientLogger() { return clientLogger; }
	private:
		static Scope<AsyncLogger> coreLogger;
		static Scope<AsyncLogger> clientLogger;
	};

}