#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera(bool roll = false) : m_RollEnabled(roll) {}

	void SetPerspective(float fov, float aspect, float near, float far);

	DirectX::XMMATRIX GetProj() const { return m_Proj; }
	DirectX::XMMATRIX GetInvProj() const { return m_ProjInv; }

	DirectX::XMMATRIX GetViewProj() const { return m_ViewProj; }
	DirectX::XMMATRIX GetInvViewProj() const { return m_ViewProjInv; }

	DirectX::XMVECTOR Project(DirectX::XMVECTOR vec);
	DirectX::XMVECTOR Unproject(DirectX::XMVECTOR vec);

	void SetWorldAngles(float pitch, float yaw, float roll);
	void SetWorldOffset(const DirectX::XMFLOAT3& offset);

	void AddRelativeOffset(const DirectX::XMFLOAT3& offset);
	void AddRelativeAngles(const DirectX::XMFLOAT3& angles);

	void UpdateMatrices();

	const DirectX::XMVECTOR& Right() const { return m_ViewInv.r[0]; }
	const DirectX::XMVECTOR& Up() const { return m_ViewInv.r[1]; }
	const DirectX::XMVECTOR& Forward() const { return m_ViewInv.r[2]; }
	const DirectX::XMVECTOR& Position() const { return m_ViewInv.r[3]; }

	DirectX::XMVECTOR& Position() { return m_ViewInv.r[3]; }
	DirectX::XMVECTOR& Rotation() { return m_Rotation; }

private:
	DirectX::XMMATRIX m_View = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX m_Proj = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX m_ViewProj = DirectX::XMMatrixIdentity();

	DirectX::XMMATRIX m_ViewInv = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX m_ProjInv = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX m_ViewProjInv = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR m_Rotation = DirectX::XMQuaternionIdentity();

	bool m_UpdatedMatrices = false;
	bool m_RollEnabled = false;
};

