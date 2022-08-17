#pragma once
#include <DirectXMath.h>
#include "Box.h"
#include "MeshIntersection.h"

namespace engine
{
	struct Ray
	{
		DirectX::XMFLOAT3 origin, direction;

		Ray() : origin(), direction() {}
		Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction) : origin(origin), direction(direction) 
		{}

		inline DirectX::XMVECTOR PointAtLine(float t) const {
			return DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&origin), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), t));
		}

		bool intersect(float t, Box box, bool val) const
		{
			DirectX::XMFLOAT3 p;
			DirectX::XMStoreFloat3(&p, PointAtLine(t));
			
			if (box.contains(p))
				return true;

			float tmin = (box.min.x - origin.x) / direction.x;
			float tmax = (box.max.x - origin.x) / direction.x;

			if (tmin > tmax) std::swap(tmin, tmax);

			float tymin = (box.min.y - origin.y) / direction.y;
			float tymax = (box.max.y - origin.y) / direction.y;

			if (tymin > tymax) std::swap(tymin, tymax);

			if ((tmin > tymax) || (tymin > tmax))
				return false;

			if (tymin > tmin)
				tmin = tymin;

			if (tymax < tmax)
				tmax = tymax;

			float tzmin = (box.min.z - origin.z) / direction.z;
			float tzmax = (box.max.z - origin.z) / direction.z;

			if (tzmin > tzmax) std::swap(tzmin, tzmax);

			if ((tmin > tzmax) || (tzmin > tmax))
				return false;

			if (tzmin > tmin)
				tmin = tzmin;

			if (tzmax < tmax)
				tmax = tzmax;

			return true;
		}

		bool intersect(MeshIntersection& nearest, const DirectX::XMFLOAT3& V1, const DirectX::XMFLOAT3& V2, const DirectX::XMFLOAT3& V3) const
		{
			using namespace DirectX;
			DirectX::XMVECTOR A, B, C, org, dir;
			A = DirectX::XMLoadFloat3(&V1);
			B = DirectX::XMLoadFloat3(&V2);
			C = DirectX::XMLoadFloat3(&V3);
			org = DirectX::XMLoadFloat3(&origin);
			dir = DirectX::XMLoadFloat3(&direction);

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
			if (t > 0 && t < nearest.t)
			{
				nearest.t = t;
				DirectX::XMStoreFloat3(&nearest.pos, PointAtLine(t));
				DirectX::XMStoreFloat3(&nearest.normal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AC, AB)));
				return true;
			}

			return false;
		}
	};
}