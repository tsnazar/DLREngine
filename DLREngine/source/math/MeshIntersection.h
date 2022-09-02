#pragma once
#include "windows/winapi.hpp"

namespace engine
{
	struct MeshIntersection
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		float near;
		float t;
		uint32_t triangle;

		constexpr void reset(float near, float far = std::numeric_limits<float>::infinity())
		{
			this->near = near;
			t = far;
		}
		bool valid() const { return std::isfinite(t); }
	};
}