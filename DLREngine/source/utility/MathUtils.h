#pragma once
#include <DirectXMath.h>

namespace engine
{
	void LoadMatrixInArray(const DirectX::XMMATRIX& matrix, DirectX::XMFLOAT4 arr[4]);
	DirectX::XMVECTOR findMaxComponent(const DirectX::XMVECTOR& vec);
}