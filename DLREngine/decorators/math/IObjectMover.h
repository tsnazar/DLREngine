#pragma once
#include <DirectXMath.h>

class IObjectMover
{
	public:
		virtual void Move(const DirectX::XMFLOAT3& offset) = 0;
};