#include "MathUtils.h"
#include <algorithm>
#include "windows/winapi.hpp"
#include <tuple>

using namespace DirectX;

namespace engine
{
	void LoadMatrixInArray(const XMMATRIX& matrix, XMFLOAT4 arr[4])
	{
		XMStoreFloat4(&arr[0], matrix.r[0]);
		XMStoreFloat4(&arr[1], matrix.r[1]);
		XMStoreFloat4(&arr[2], matrix.r[2]);
		XMStoreFloat4(&arr[3], matrix.r[3]);
	}

	XMVECTOR FindMaxComponent(const XMVECTOR& vec)
	{
		return XMVectorReplicate((std::max)((std::max)(XMVectorGetX(vec), XMVectorGetY(vec)), XMVectorGetZ(vec)));
	}

	DirectX::XMFLOAT3 RadianceFromDistance(float distance, float radius, DirectX::XMFLOAT3 irradiance)
	{
		float angleSin = std::min(1.f, radius / distance);
		float angleCos = sqrtf(1.f - angleSin * angleSin);
		float occupation = 1.f - angleCos;
		return DirectX::XMFLOAT3(irradiance.x / occupation, irradiance.y / occupation, irradiance.z / occupation);
	}

	float MaxIlluminationDistance(float radius, DirectX::XMFLOAT3 radiance)
	{
		float maxComponent = std::max(std::max(radiance.x, radiance.y), radiance.z);
		
		float distance = radius / sqrtf(1 - powf((1 - 0.01 / maxComponent), 2));

		return distance;
	}

	float Length(const DirectX::XMFLOAT3& vec)
	{
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	float Dot(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	float Lerp(float a, float b, float s)
	{
		return a + s * (b - a);
	}

	DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& vec)
	{
		float length = Length(vec);
		return { vec.x / length, vec.y / length, vec.z / length };
	}
}

namespace DirectX
{
	DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return DirectX::XMFLOAT3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}
	DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return DirectX::XMFLOAT3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
	}
	DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return DirectX::XMFLOAT3{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
	}
	DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return DirectX::XMFLOAT3{ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
	}
	DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, const float& rhs)
	{
		return DirectX::XMFLOAT3{ lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
	}
	DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, const float& rhs)
	{
		return DirectX::XMFLOAT3{ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
	}
	DirectX::XMFLOAT3 operator/(const float& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return DirectX::XMFLOAT3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
	}
	DirectX::XMFLOAT3 operator*(const float& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return DirectX::XMFLOAT3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
	}
	DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& vec)
	{
		return DirectX::XMFLOAT3{ -vec.x, -vec.y, -vec.z };
	}
	bool operator<(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return std::tie(lhs.x, lhs.y, lhs.z) < std::tie(rhs.x, rhs.y, rhs.z);
	}
	bool operator==(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return std::tie(lhs.x, lhs.y, lhs.z) == std::tie(rhs.x, rhs.y, rhs.z);
	}
	bool operator<=(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
	{
		return (lhs < rhs) || (lhs == rhs);
	}
	DirectX::XMFLOAT3 Min(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
	{
		return { a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z };
	}

	DirectX::XMFLOAT3 Max(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
	{
		return { a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z };
	}
}