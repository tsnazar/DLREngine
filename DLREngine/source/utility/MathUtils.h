#pragma once
#include <DirectXMath.h>
#include <array>

namespace engine
{

	void LoadMatrixInArray(const DirectX::XMMATRIX& matrix, DirectX::XMFLOAT4 arr[4]);
	DirectX::XMVECTOR FindMaxComponent(const DirectX::XMVECTOR& vec);
	DirectX::XMFLOAT3 RadianceFromDistance(float distance, float radius, DirectX::XMFLOAT3 irradiance);

	float Length(const DirectX::XMFLOAT3& vec);
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
}