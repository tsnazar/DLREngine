#pragma once
#include "IObjectMover.h"
#include <DirectXMath.h>

class Vector3Mover : public IObjectMover
{
public:
	DirectX::XMFLOAT3& vec;

	Vector3Mover(DirectX::XMFLOAT3& vec) : vec(vec) {}
	virtual void Move(const DirectX::XMFLOAT3& offset) override
	{
		vec.x += offset.x;
		vec.y += offset.y;
		vec.z += offset.z;
	}
};