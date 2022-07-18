#pragma once
#include <DirectXMath.h>
#include <limits>
#include "Material.h"

namespace math
{
	struct Intersection
	{
		float t;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;

		void Reset() { t = std::numeric_limits<float>::infinity(); }
	};
}