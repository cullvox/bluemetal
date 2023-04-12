#pragma once

#include "Core/Log.hpp"

#define BL_CHECK(expr, error) \
	if (not (expr)) \
	{ \
		Logger::Error(error); \
		return false; \
	}

#define BL_CHECK_GOTO(expr, error, go_to) \
	if (not (expr)) \
	{ \
		Logger::Error(error); \
		goto go_to; \
	}

#define BL_CHECK_NL(expr) \
	if (not (expr)) \
		return false;

#define BL_CHECK_NR(expr) \
	if (not (expr)) \
		return;

#define BL_CHECK_SR(expr, error, ret) \
	if (not (expr)) \
	{ \
		Logger::Error(error); \
		return ret; \
	}

#define BIT(x) 1 << x