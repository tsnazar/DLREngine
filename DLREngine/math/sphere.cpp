#include "sphere.h"

bool math::sphere::hit(const ray& casted_ray, float t_min, float t_max, hit_record& record) const
{
	DirectX::XMVECTOR centerVec = DirectX::XMLoadFloat3(&center);
	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&casted_ray.direction);
	DirectX::XMVECTOR oc = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&casted_ray.origin), centerVec);
	float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dirVec, dirVec));
	float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dirVec, oc));
	float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(oc, oc)) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0)
	{
		float sqrtDiscriminant = sqrt(discriminant);
		float temp = (-b > sqrtDiscriminant) ? (-b - sqrtDiscriminant) / a : (-b + sqrtDiscriminant) / a;
		if (temp < t_max && temp > t_min)
		{
			record.t = temp;
			casted_ray.point_at_line(record.point, temp);
			DirectX::XMStoreFloat3(&record.normal, DirectX::XMVectorScale(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&record.point), centerVec), 1.0f/radius));
			return true;
		}
	}
	return false;
}