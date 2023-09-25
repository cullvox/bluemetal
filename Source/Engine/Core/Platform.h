#pragma once

#if defined(_WIN32)
	#define BLUEMETAL_SYSTEM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#define BLUEMETAL_SYSTEM_IOS
	#else
		#define BLUEMETAL_SYSTEM_MACOS
	#endif
#elif defined(__unix__)
	#if defined(__linux__)
		#define BLUEMETAL_SYSTEM_LINUX
	#else
		#error Unknown UNIX Operating System, BloodLust cannot properly build on it yet.
	#endif
#else
	#error Unknown Operating System, BloodLust cannot properly build on it yet.
#endif