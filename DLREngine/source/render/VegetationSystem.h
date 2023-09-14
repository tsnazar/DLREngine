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
				OPACITY_TEXTURE = 8, AO_TEXTURE = 9, TRANSLUCENCY_TEXTURE = 10, TARGET_DIMENSIONS_CONSTANTS = 4,
				INSTANCE_BUFFER = 0, WIND_BUFFER = 4,
				MESH_DS_BUFFER = 0, INSTANCE_DS_BUFFER = 1,
				SHADOWMAP_MATRICES = 1, SHADOWMAP_DIMENSIONS = 3,
				DEPTH_DS_TEXTURE = 0, ALBEDO_DS_TEXTURE = 1, NORMALS_DS_TEXTURE = 2, ROUGHMETALLIC_DS_TEXTURE = 3, EMISSION_DS_TEXTURE = 5,
				EMISSION_GB_TEXTURE = 4,
				STENCIL_REF = 2,
			};
		};
	public:
		struct GrassTextures
		{
			Texture2D* albedo = nullptr;
			Texture2D* roughness = nullptr;
			Texture2D* metallic = nullptr;
			Texture2D* normals = nullptr;
			Texture2D* opacity = nullptr;
			Texture2D* ao = nullptr;
			Texture2D* translucency = nullptr;

			GrassTextures(){}
			GrassTextures(Texture2D* albedo, Texture2D* roughness, Texture2D* metallic,
				Texture2D* normals, Texture2D* opacity, Texture2D* ao, Texture2D* translucency) :
				albedo(albedo), roughness(roughness), metallic(metallic), normals(normals), opacity(opacity), ao(ao), translucency(translucency)
			{
			}

			bool IsValid() 
			{
				return  !(albedo == nullptr || roughness == nullptr || metallic == nullptr || normals == nullptr || opacity == nullptr || ao == nullptr || translucency == nullptr);
			}

		};

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

		void SetShaders(Shader* forwardShader, Shader* deferredShader, Shader* deferredIBLShader, Shader* GBufferShader, Shader* shadowsShader)
		{
			m_ForwardShader = forwardShader;
			m_DeferredShader = deferredShader;
			m_DeferredIBLShader = deferredIBLShader;
			m_GBufferShader = GBufferShader;
			m_ShadowsShader = shadowsShader;
		}

		void SetTextures(GrassTextures textures) { m_Textures = textures; }

		void CreateField(const DirectX::XMFLOAT3& pos, float halflength, float radius, float minSize, float maxSize, float windAngle);

		void Render(Sky::IblResources iblResources);

		void RenderToGBuffer();

		void ResolveGBuffer(Sky::IblResources iblResources, Texture2D& depth, Texture2D& albedo, Texture2D& normals,
			Texture2D& roughnessMetallic, Texture2D& emission, ConstantBuffer& dimensions);

		void RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights);

		static VegetationSystem& Get() { return *s_Instance; }
	private:
		DirectX::XMFLOAT3 m_Center;
		float m_Length;
		float m_Radius;
		VertexBuffer m_InstanceBuffer;
		ConstantBuffer m_WindBuffer;
	
		Shader* m_ForwardShader;
		Shader* m_DeferredShader;
		Shader* m_DeferredIBLShader;
		Shader* m_GBufferShader;
		Shader* m_ShadowsShader;

		GrassTextures m_Textures;

	private:
		static VegetationSystem* s_Instance;
	};
}