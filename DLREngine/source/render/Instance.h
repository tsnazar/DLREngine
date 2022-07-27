#pragma once
#include <DirectXMath.h>

namespace engine
{
	enum class InstanceType
	{
		Undefined,
		Transform
	};

	struct InstanceTransform
	{
		DirectX::XMFLOAT4 matrix[4];
	};
}