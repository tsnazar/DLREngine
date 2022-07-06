#pragma once

#define ALWAYS_ASSERT(expr)\
	if(!(expr))\
		__debugbreak()\

