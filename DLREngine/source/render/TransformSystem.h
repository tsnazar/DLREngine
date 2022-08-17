#pragma once
#include <DirectXMath.h>
#include "Debug.h"
#include "SolidVector.h"

namespace engine
{
	class TransformSystem
	{
	public:
		struct Transform
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 scale;
			DirectX::XMFLOAT3 rotation;

			DirectX::XMMATRIX GetMatrix() const
			{
				//DirectX::XMFLOAT4X4 res;
				DirectX::XMMATRIX mat = DirectX::XMMatrixAffineTransformation(DirectX::XMLoadFloat3(&scale),
					DirectX::XMVectorZero(),
					DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z),
					DirectX::XMLoadFloat3(&position));

				//DirectX::XMStoreFloat4x4(&res, mat);
				return mat;
			}

			DirectX::XMMATRIX GetTranspose() const
			{
				//DirectX::XMFLOAT4X4 res;
				DirectX::XMMATRIX mat = DirectX::XMMatrixAffineTransformation(DirectX::XMLoadFloat3(&scale),
					DirectX::XMVectorZero(),
					DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z),
					DirectX::XMLoadFloat3(&position));
				mat = DirectX::XMMatrixTranspose(mat);
				//DirectX::XMStoreFloat4x4(&res, mat);
				return mat;
			}
			 
			DirectX::XMMATRIX GetInvMatrix() const
			{
				//DirectX::XMFLOAT4X4 res;
				DirectX::XMMATRIX mat = DirectX::XMMatrixAffineTransformation(DirectX::XMLoadFloat3(&scale),
					DirectX::XMVectorZero(),
					DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z),
					DirectX::XMLoadFloat3(&position));
				mat = DirectX::XMMatrixInverse(nullptr, mat);
				//DirectX::XMStoreFloat4x4(&res, mat);
				return mat;
			}
		};
	public:
		static void Init();

		static void Fini();

		static TransformSystem& Get() { return *s_Instance; }

		SolidVector<Transform>& GetTransforms() { return m_Transforms; }

	private:
		SolidVector<Transform> m_Transforms;
	private:
		static TransformSystem* s_Instance;
	};
}