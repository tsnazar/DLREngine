#pragma once
#include <DirectXMath.h>

namespace engine
{
	struct TransformComponent
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 scale;
	};
}