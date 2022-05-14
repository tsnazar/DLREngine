#include "Camera.h"

void Camera::SetPerspective(float fov, float aspect, float near, float far)
{
	m_Proj = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, near, far);
	m_ProjInv = DirectX::XMMatrixInverse(nullptr, m_Proj);

	m_ViewProj = m_View * m_Proj;
	m_ViewProjInv = m_ProjInv * m_ViewInv;
}

void Camera::SetWorldAngles(float pitch, float yaw, float roll)
{
	m_UpdatedMatrices = false;
	m_Rotation = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
}

void Camera::SetWorldOffset(const DirectX::XMFLOAT3& offset)
{
	m_UpdatedMatrices = false;
	Position() = DirectX::XMLoadFloat3(&offset);
}

void Camera::AddRelativeOffset(const DirectX::XMFLOAT3& offset)
{
	m_UpdatedMatrices = false;
	Position() = DirectX::XMVectorAdd(Position(), DirectX::XMVectorScale(Right(), offset.x));
	Position() = DirectX::XMVectorAdd(Position(), DirectX::XMVectorScale(Up(), offset.y));
	Position() = DirectX::XMVectorAdd(Position(), DirectX::XMVectorScale(Forward(), offset.z));
}

void Camera::AddRelativeAngles(const DirectX::XMFLOAT3& angles)
{
	m_UpdatedMatrices = false;
	m_Rotation = DirectX::XMQuaternionMultiply(m_Rotation, DirectX::XMQuaternionRotationAxis(Forward(), angles.z));
	m_Rotation = DirectX::XMQuaternionMultiply(m_Rotation, DirectX::XMQuaternionRotationAxis(Right(), angles.y));
	m_Rotation = DirectX::XMQuaternionMultiply(m_Rotation, DirectX::XMQuaternionRotationAxis(Up(), angles.x));
	m_Rotation = DirectX::XMQuaternionNormalize(m_Rotation);
}

void Camera::UpdateMatrices()
{
	if (m_UpdatedMatrices) return;
	m_UpdatedMatrices = true;

	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(m_Rotation);
	const DirectX::XMVECTOR pos = Position();

	m_ViewInv = mat;
	Position() = pos;

	m_View = DirectX::XMMatrixTranspose(mat);
	m_View.r[3] = DirectX::XMVectorNegate(DirectX::XMVector3Transform(pos, m_View));
	m_View.r[3] = DirectX::XMVectorSetW(m_View.r[3], 1.0f);

	m_ViewProj = m_View * m_Proj;
	m_ViewProjInv = m_ProjInv * m_ViewInv;
}
