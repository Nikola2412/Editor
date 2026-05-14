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

#if DEBUG | RELEASE
#define CORE_INFO(...)		Log::GetCoreLogger()->Info(__VA_ARGS__)
#define CORE_WARN(...)		Log::GetCoreLogger()->Warn(__VA_ARGS__)
#define CORE_ERROR(...)		Log::GetCoreLogger()->Error(__VA_ARGS__)

#define INFO(...)			Log::GetClientLogger()->Info(__VA_ARGS__)
#define WARN(...)			Log::GetClientLogger()->Warn(__VA_ARGS__)
#define ERROR(...)			Log::GetClientLogger()->Error(__VA_ARGS__)
#else

#define CORE_INFO(...)	
#define CORE_WARN(...)	
#define CORE_ERROR(...)	

#define INFO(...)		
#define WARN(...)		
#define ERROR(...)		
#endif // PH_DEBUG

}