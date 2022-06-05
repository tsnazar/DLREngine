#pragma once
#include <DirectXMath.h>

namespace math
{
	struct Transform
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4 rotation;
		DirectX::XMFLOAT3 position;

		Transform() {}
		Transform(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position) : scale(scale), rotation(rotation), position(position) {}
		DirectX::XMMATRIX ToMatrix() const { return DirectX::XMMatrixAffineTransformation(DirectX::XMLoadFloat3(&scale), DirectX::XMVectorZero(), DirectX::XMLoadFloat4(&rotation), DirectX::XMLoadFloat3(&position)); }
		DirectX::XMMATRIX ToInvMatrix() const 
		{
			DirectX::XMVECTOR s = DirectX::XMVectorDivide(DirectX::XMVectorReplicate(1.0f), DirectX::XMLoadFloat3(&scale));
			DirectX::XMVECTOR r = DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&rotation));
			DirectX::XMVECTOR p = DirectX::XMVectorMultiply(DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&position)), s);
			return DirectX::XMMatrixAffineTransformation(s, DirectX::XMVectorZero(), r, p);
		}
	};
}