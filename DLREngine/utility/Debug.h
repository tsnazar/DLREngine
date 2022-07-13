#pragma once
#include <iostream>

#define ALWAYS_ASSERT(expr)\
	if(!(expr)){\
		std::cout << "Error: " << __FILE__ << " " << __LINE__ << std::endl;\
		__debugbreak();}\

