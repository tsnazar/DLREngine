#include "Plane.h"

bool math::Plane::Intersect(const Ray& ray, Intersection& record) const
{
	DirectX::XMVECTOR originVec = DirectX::XMLoadFloat3(&ray.origin);
	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&ray.direction);
	DirectX::XMVECTOR pointVec = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR normalVec = DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&normal));

	DirectX::XMVECTOR denom = DirectX::XMVector3Dot(dirVec, normalVec);
	if (DirectX::XMVectorGetX(denom) > 0)
	{
		DirectX::XMVECTOR top = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(pointVec, originVec), normalVec);
		float t = DirectX::XMVectorGetX(DirectX::XMVectorDivide(top, denom));
		if (t < record.t && t > 0.0f)
		{
			record.t = t;
			DirectX::XMStoreFloat3(&record.pos, ray.PointAtLine(t));
			record.normal = normal;
			return true;
		}
	}
	return false;
}