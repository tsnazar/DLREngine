#pragma once
#include <DirectXMath.h>
#include <limits>

namespace engine
{
	struct Query
	{
		float t;
		uint32_t transformID;
		uint32_t meshID = 0;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		bool usable;

		void reset()
		{
			t = std::numeric_limits<float>::infinity();
			pos = { 0,0,0 };
			normal = { 0,0,0 };
			usable = false;
		}
	};
}