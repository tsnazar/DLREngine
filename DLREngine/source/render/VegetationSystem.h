#pragma once
#include <DirectXMath.h>
#include <vector>

#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "Sky.h"
#include "LightSystem.h"

namespace engine
{
	class VegetationSystem
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t {
				ALBEDO_TEXTURE = 0, ROUGHNESS_TEXTURE = 1, METALLIC_TEXTURE = 2, NORMAL_MAP_TEXTURE = 3,
				SHADOWMAP_TEXTURE = 4, IRRADIANCE_TEXTURE = 5, REFLECTION_TEXTURE = 6, REFLECTANCE_TEXTURE = 7, 
				OPACITY_TEXTURE = 8, AO_TEXTURE = 9, TRANSLUCENCY_TEXTURE = 10, 
				INSTANCE_BUFFER = 0, WIND_BUFFER = 4,
				SHADOWMAP_MATRICES = 1, SHADOWMAP_DIMENSIONS = 3
			};
		};
	public:
		struct Instance
		{
			DirectX::XMFLOAT3 pos;
			float scale;
			DirectX::XMFLOAT2 rot[2];
		};

		struct Wind
		{
			DirectX::XMFLOAT2 rot[2];
			DirectX::XMFLOAT2 invRot[2];
		};

	public:
		static void Init();

		static void Fini();

		void CreateField(const DirectX::XMFLOAT3& pos, float halflength, float radius, float minSize, float maxSize, float windAngle);

		void Render(Sky::IblResources iblResources);

		void RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights);

		static VegetationSystem& Get() { return *s_Instance; }
	private:
		DirectX::XMFLOAT3 m_Center;
		float m_Length;
		float m_Radius;
		VertexBuffer m_InstanceBuffer;
		ConstantBuffer m_WindBuffer;
	
	private:
		static VegetationSystem* s_Instance;
	};
}