#pragma once
#include <DirectXMath.h>
#include <array>

namespace engine
{

	void LoadMatrixInArray(const DirectX::XMMATRIX& matrix, DirectX::XMFLOAT4 arr[4]);
	DirectX::XMVECTOR FindMaxComponent(const DirectX::XMVECTOR& vec);
	DirectX::XMFLOAT3 RadianceFromDistance(float distance, float radius, DirectX::XMFLOAT3 irradiance);
	float MaxIlluminationDistance(float radius, DirectX::XMFLOAT3 radiance);
	float Length(const DirectX::XMFLOAT3& vec);
	float Dot(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
	float Lerp(float a, float b, float s);
	DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& vec);
}

namespace DirectX
{
	DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator-(const DirectX::XMFLOAT3& vec);
	DirectX::XMFLOAT3 operator/(const DirectX::XMFLOAT3& lhs, const float& rhs);
	DirectX::XMFLOAT3 operator*(const DirectX::XMFLOAT3& lhs, const float& rhs);
	DirectX::XMFLOAT3 operator/(const float& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 operator*(const float& lhs, const DirectX::XMFLOAT3& rhs);
	bool operator<(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	bool operator==(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	bool operator<=(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);
	DirectX::XMFLOAT3 Min(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
	DirectX::XMFLOAT3 Max(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b);
}