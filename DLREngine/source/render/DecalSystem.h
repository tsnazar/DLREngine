#pragma once
#include <DirectXMath.h>

#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "DepthTarget.h"

namespace engine
{
	class DecalSystem
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t {
				DEPTH_GB_TEXTURE = 0, NORMALS_GB_TEXTURE = 1, DECAL_GB_TEXTURE = 2, OBJECTIDS_GB_TEXTURE = 3, 
				TARGET_DIMENSIONS_CONSTANTS = 2, MESH_BUFFER = 0, INSTANCE_BUFFER = 1,
				STENCIL_REF = 1,
			};
		};
	public:
		static const float MAX_DEPTH;
		static const float MIN_DEPTH;
		
		struct Instance
		{
			DirectX::XMMATRIX decalToModel;
			DirectX::XMFLOAT3 color;
			uint32_t transformID;
			uint32_t objectID;
			DirectX::XMFLOAT3 decalRight;
		};

		struct GpuInstance
		{
			DirectX::XMFLOAT4 decalToWorld[4];
			DirectX::XMFLOAT4 worldToDecal[4];
			DirectX::XMFLOAT3 color;
			uint32_t objectID;
			DirectX::XMFLOAT3 decalRight;
		};

	public:
		static void Init();

		static void Fini();

		void SetShaders(Shader* GBufferShader) { m_GBufferShader = GBufferShader; }

		void SetTextures(Texture2D* decalTexture) { m_DecalTexture = decalTexture; }

		static DecalSystem& Get() { return *s_Instance; }

		void Update();

		void SpawnDecal(Camera& camera, DirectX::XMFLOAT3 position, float halfSize, float NdotR, uint32_t transformID, uint32_t objectID);

		void RenderToGBuffer(Texture2D& depth, ConstantBuffer& dimensions, Texture2D& normals, Texture2D& objectIDs);

	private:
		std::vector<Instance> m_Instances;

		VertexBuffer m_InstanceBuffer;

		Shader* m_GBufferShader = nullptr;
		Texture2D* m_DecalTexture = nullptr;

	private:
		static DecalSystem* s_Instance;
	};
}