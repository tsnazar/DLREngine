#pragma once
#include <DirectXMath.h>

namespace math
{
	struct hit_record
	{
		float t;
		DirectX::XMFLOAT3 point;
		DirectX::XMFLOAT3 normal;
	};
}