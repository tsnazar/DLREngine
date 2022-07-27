#pragma once
#include <xhash>

namespace engine
{
	template <class T>
	inline void hashCombine(std::size_t& s, const T& v)
	{
		std::hash<T> h;
		s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
	}
}