#pragma once
#include "IObjectMover.h"
#include "Transform.h"

class TransformMover : public IObjectMover
{
public:
	math::Transform& transform;

	TransformMover(math::Transform& transform) : transform(transform) {}

	virtual void Move(const DirectX::XMFLOAT3& offset) override
	{
		transform.position.x += offset.x;
		transform.position.y += offset.y;
		transform.position.z += offset.z;
	}
};