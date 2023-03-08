#pragma once

#if defined(_WIN32)
	#define BLOODLUST_SYSTEM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		#define BLOODLUST_SYSTEM_IOS
	#else
		#define BLOODLUST_SYSTEM_MACOS
	#endif
#elif defined(__unix__)
	#if defined(__linux__)
		#define BLOODLUST_SYSTEM_LINUX
	#else
		#error Unknown UNIX Operating System, BloodLust cannot properly build on it yet.
	#endif
#else
	#error Unknown Operating System, BloodLust cannot properly build on it yet.
#endif

#ifdef BLOODLUST_SYSTEM_WINDOWS
	#define BLOODLUST_API_EXPORT __declspec(dllexport)
	#define BLOODLUST_API_IMPORT __declspec(dllimport)
#else
	#define BLOODLUST_API_EXPORT __attribute__((__visibility__("default")))
	#define BLOODLUST_API_IMPORT __attribute__((__visibility__("default")))
#endif