#pragma once

#include "Core.h"

#include <AsyncLogger.h>

namespace Editor
{
	class Log
	{
	public:
		static void Init();
		inline static Ref<AsyncLogger>& GetCoreLogger() { return coreLogger; }
		inline static Ref<AsyncLogger>& GetClientLogger() { return clientLogger; }
	private:
		static Ref<AsyncLogger> coreLogger;
		static Ref<AsyncLogger> clientLogger;
	};

}