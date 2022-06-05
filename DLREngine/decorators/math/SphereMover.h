#pragma once
#include "IObjectMover.h"
#include "geometry/Sphere.h"

class SphereMover : public IObjectMover
{
public:
	math::Sphere& sphere;

	SphereMover(math::Sphere& sphere) : sphere(sphere) {}

	virtual void Move(const DirectX::XMFLOAT3& offset) override
	{
		sphere.center.x += offset.x;
		sphere.center.y += offset.y;
		sphere.center.z += offset.z;
	}
};