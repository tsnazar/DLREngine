#include "Sphere.h"

bool math::Sphere::Intersect(const Ray& ray, Intersection& record) const
{
	DirectX::XMVECTOR centerVec = DirectX::XMLoadFloat3(&center);
	DirectX::XMVECTOR originVec = DirectX::XMLoadFloat3(&ray.origin);
	DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&ray.direction);
	DirectX::XMVECTOR oc = DirectX::XMVectorSubtract(originVec, centerVec);

	float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dirVec, dirVec));
	float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dirVec, oc));
	float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(oc, oc)) - radius * radius;
	float discriminant = b * b - a * c;
	
	if (discriminant > 0)
	{
		float sqrtDiscriminant = sqrt(discriminant);
		float temp = (-b > sqrtDiscriminant) ? (-b - sqrtDiscriminant) / a : (-b + sqrtDiscriminant) / a;
		if (temp < record.t && temp > 0.0f)
		{
			DirectX::XMVECTOR posOnLine = ray.PointAtLine(temp);
			record.t = temp;
			DirectX::XMStoreFloat3(&record.pos, posOnLine);
			DirectX::XMStoreFloat3(&record.normal, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(posOnLine, centerVec)));
			return true;
		}
	}
	return false;
}