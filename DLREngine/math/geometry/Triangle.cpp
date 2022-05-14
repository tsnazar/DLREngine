#include "Triangle.h"

bool math::Triangle::Intersect(const Ray& ray, Intersection& record) const
{
	DirectX::XMVECTOR A, B, C, org, dir;
	A = DirectX::XMLoadFloat3(vertices);
	B = DirectX::XMLoadFloat3(vertices + 1);
	C = DirectX::XMLoadFloat3(vertices + 2);
	org = DirectX::XMLoadFloat3(&ray.origin);
	dir = DirectX::XMLoadFloat3(&ray.direction);

	DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
	DirectX::XMVECTOR AC = DirectX::XMVectorSubtract(C, A);
	DirectX::XMVECTOR pVec = DirectX::XMVector3Cross(dir, AB);
	DirectX::XMVECTOR det = DirectX::XMVector3Dot(AC, pVec);

	if (DirectX::XMVectorGetX(det) < 0) return false;

	DirectX::XMVECTOR invDet = DirectX::XMVectorDivide(DirectX::XMVectorReplicate(1.0f), det);

	DirectX::XMVECTOR tVec = DirectX::XMVectorSubtract(org, A);
	float u = DirectX::XMVectorGetX(DirectX::XMVectorMultiply(DirectX::XMVector3Dot(tVec, pVec), invDet));
	if (u < 0 || u > 1) return false;

	DirectX::XMVECTOR qVec = DirectX::XMVector3Cross(tVec, AC);
	float v = DirectX::XMVectorGetX(DirectX::XMVectorMultiply(DirectX::XMVector3Dot(dir, qVec), invDet));
	if (v < 0 || v + u > 1) return false;

	float t = DirectX::XMVectorGetX(DirectX::XMVectorMultiply(DirectX::XMVector3Dot(AB, qVec), invDet));
	if (t > 0 && t < record.t)
	{
		record.t = t;
		DirectX::XMStoreFloat3(&record.pos, ray.PointAtLine(t));
		DirectX::XMStoreFloat3(&record.normal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AC, AB)));
		return true;
	}

	return false;
}
