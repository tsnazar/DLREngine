#pragma once
#include <DirectXMath.h>
#include <limits>

namespace engine
{
	struct Query
	{
		float t;
		uint32_t transform;
		DirectX::XMFLOAT3 pos;
		bool usable;

		void reset()
		{
			t = std::numeric_limits<float>::infinity();
			pos = { 0,0,0 };
			usable = false;
		}
	};
}