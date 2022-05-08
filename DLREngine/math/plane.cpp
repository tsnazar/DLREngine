#include "plane.h"

bool math::plane::hit(const ray& casted_ray, float t_min, float t_max, hit_record& record) const
{
	DirectX::XMVECTOR originVec = DirectX::XMLoadFloat3(&casted_ray.origin);
	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&casted_ray.direction);
	DirectX::XMVECTOR pointVec = DirectX::XMLoadFloat3(&point);
	DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&normal);

	DirectX::XMVECTOR denom = DirectX::XMVector3Dot(dirVec, normalVec);
	if (DirectX::XMVectorGetX(denom) > 0)
	{
		DirectX::XMVECTOR top = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(pointVec, originVec), normalVec);
		float t = DirectX::XMVectorGetX(DirectX::XMVectorDivide(top, denom));

		if (t < t_max && t > t_min)
		{
			record.t = t;
			DirectX::XMStoreFloat3(&record.point, casted_ray.point_at_line(t));
			DirectX::XMStoreFloat3(&record.normal, DirectX::XMVectorNegate(normalVec));
			return true;
		}
	}
	return false;
}