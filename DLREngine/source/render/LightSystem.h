#pragma once
#include <DirectXMath.h>
#include <array>
#include <vector>
#include "ConstantBuffer.h"
#include "MathUtils.h"
#include "DepthTarget.h"
#include "RenderTarget.h"
//#include "Renderer.h"
#include "VertexBuffer.h"

namespace engine
{
	class LightSystem
	{
	public:
		static const uint8_t MAX_POINT_LIGHTS = 2;
		static const uint32_t SHADOW_MAP_WIDTH = 512;
		static const uint32_t SHADOW_MAP_HEIGHT = 512;
		static const float SHADOW_MAP_ASPECT;
		static const float SHADOW_MAP_NEAR;
		static const float SHADOW_MAP_FAR;
		static const DirectX::XMMATRIX SHADOW_MAP_PROJECTION;

		struct GpuPointLight
		{
			GpuPointLight() = default;
			GpuPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 irradiance, float radius, float distance) : pos(position), rad(radius), radiance(RadianceFromDistance(distance, radius, irradiance)), dist(MaxIlluminationDistance(radius, radiance)) {};

			DirectX::XMFLOAT3 pos;
			float rad;
			DirectX::XMFLOAT3 radiance;
			float dist;
		};

		struct ShadowMapDimensions
		{
			uint32_t width;
			uint32_t height;
			DirectX::XMFLOAT2 padding;
		};

		struct ShadowMapMatrices
		{
			DirectX::XMFLOAT4X4 matrices[6];
		};

		struct ShadowMapGeometryShaderConstants
		{
			DirectX::XMFLOAT4X4 matrices[6];
			uint32_t sliceOffset;
			DirectX::XMFLOAT3 padding;
		};

		struct PointLight
		{
			DirectX::XMFLOAT3 radiance;
			uint32_t transformId;
			float radius;
			float dist;
		};

	public:

		LightSystem();

		static void Init();

		static void Fini();

		static LightSystem& Get() { return *s_Instance; }

		void AddPointLight(const GpuPointLight& light);

		void Update();

		void RenderToShadowMaps();

		void InitShadowMaps();

		void ResolveGBuffer(DepthTarget& depth, RenderTarget& albedo, RenderTarget& normals, 
						RenderTarget& roughnessMetallic, RenderTarget& emission, RenderTarget& position, ConstantBuffer& dimensions);

		Texture2D& GetShadowMap() { return m_ShadowMap; }

		std::vector<ShadowMapMatrices>& GetShadowMatrices() { return m_Matrices; }

		ConstantBuffer& GetShadowMatricesBuffer() { return m_ShadowMatricesBuffer; }

		ConstantBuffer& GetShadowMatrixBuffer() { return m_ShadowMatrixBuffer; }

		ConstantBuffer& GetShadowMapDimensions() { return m_ShadowMapDimensions; }

		VertexBuffer& GetDeferedShadingLightInstances() { return m_LightInstances; }

		uint32_t GetNumberOfLights() { return m_NumLights; }

	protected:
		void GenerateShadowTransforms(DirectX::XMFLOAT4X4* arr,const DirectX::XMFLOAT3& position);

	private:
		uint32_t m_NumLights = 0;
		std::array<PointLight, MAX_POINT_LIGHTS> m_PointLightRefs;
		DepthTarget m_ShadowMap;
		std::vector<ShadowMapMatrices> m_Matrices;
		ConstantBuffer m_ShadowMatrixBuffer;
		ConstantBuffer m_ShadowMatricesBuffer;
		ConstantBuffer m_ShadowMapDimensions;
		
		VertexBuffer m_LightInstances;

		bool m_ResizeLightInstances = false;
	private:
		static LightSystem* s_Instance;
	};
}