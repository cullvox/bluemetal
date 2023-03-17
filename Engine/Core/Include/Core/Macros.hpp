#pragma once

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