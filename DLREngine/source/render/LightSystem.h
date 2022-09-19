#pragma once
#include <DirectXMath.h>
#include <array>
#include <vector>
#include "ConstantBuffer.h"
#include "MathUtils.h"
#include "DepthTarget.h"

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
			GpuPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 irradiance, float radius, float distance) : position(position), radius(radius), radiance(RadianceFromDistance(distance, radius, irradiance)) {};

			DirectX::XMFLOAT3 position;
			float radius;
			DirectX::XMFLOAT3 radiance;
			float placeholder;
		};

		struct ShadowMapConstants
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
		};

		struct ShaderDescription
		{
			enum Bindings : uint32_t {
				SHADOWMAP_MATRICES = 1,
			};
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

		Texture2D& GetShadowMap() { return m_ShadowMap; }

		ConstantBuffer& GetShadowMatrices() { return m_ShadowMatricesBuffer; }

	protected:
		void GenerateShadowTransforms(DirectX::XMFLOAT4X4* arr,const DirectX::XMFLOAT3& position);

	private:
		uint32_t m_NumLights = 0;
		std::array<PointLight, MAX_POINT_LIGHTS> m_PointLightRefs;
		DepthTarget m_ShadowMap;
		std::vector<ShadowMapConstants> m_Matrices;
		ConstantBuffer m_ShadowMatrixBuffer;
		ConstantBuffer m_ShadowMatricesBuffer;
	private:
		static LightSystem* s_Instance;
	};
}