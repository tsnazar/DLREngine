#pragma once
#include <DirectXMath.h>
#include <array>
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

		struct PointLight
		{
			PointLight() = default;
			PointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 irradiance, float radius, float distance) : position(position), radius(radius), radiance(RadianceFromDistance(distance, radius, irradiance)) {};

			DirectX::XMFLOAT3 position;
			float radius;
			DirectX::XMFLOAT3 radiance;
			float placeholder;
		};

		struct ShadowMapConstants
		{
			DirectX::XMFLOAT4X4 matrices[6];
			DirectX::XMFLOAT3 lightPos;
			float farPlane;
		};

		struct PointLightRef
		{
			DirectX::XMFLOAT3 radiance;
			uint32_t transformId;
			float radius;
			DirectX::XMFLOAT4X4 matrices[6];
		};

	public:

		LightSystem();

		static void Init();

		static void Fini();

		static LightSystem& Get() { return *s_Instance; }

		void AddPointLight(const PointLight& light);

		void Update();

		void RenderToShadowMaps();

		void InitShadowMaps();

		Texture2D& GetShadowMap() { return m_ShadowMaps[0]; }

	protected:
		void GenerateShadowTransforms(DirectX::XMFLOAT4X4* arr,const DirectX::XMFLOAT3& position);

	private:
		uint32_t m_NumLights = 0;
		std::array<PointLightRef, MAX_POINT_LIGHTS> m_PointLightRefs;
		std::array<DepthTarget, MAX_POINT_LIGHTS> m_ShadowMaps;
		ConstantBuffer m_ShadowMatricesBuffer;
	private:
		static LightSystem* s_Instance;
	};
}