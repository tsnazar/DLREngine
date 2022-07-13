#pragma once
#include <type_traits>

namespace engine
{
	using KeyCode = uint16_t;

	namespace Key
	{
		enum : KeyCode
		{
			LMB = 0x01,
			RMB = 0x02,
			SHIFT = 0x10,
			CONTROL = 0x11,
			SPACE = 0x20,
			A = 0x41,
			D = 0x44,
			E = 0x45,
			G = 0x47,
			Q = 0x51,
			R = 0x52,
			S = 0x53,
			W = 0x57,
			OEM_PLUS = 0xBB,
			OEM_MINUS = 0xBD
		};
	};
}