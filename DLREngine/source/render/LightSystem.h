#pragma once
#include <DirectXMath.h>
#include <array>
#include "ConstantBuffer.h"
#include "MathUtils.h"

namespace engine
{
	class LightSystem
	{
	public:
		static const uint8_t MAX_POINT_LIGHTS = 1;
		
		struct PointLight
		{
			PointLight() = default;
			PointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 irradiance, float radius, float distance) : position(position), radius(radius), radiance(RadianceFromDistance(distance, radius, irradiance)) {};

			DirectX::XMFLOAT3 position;
			float radius;
			DirectX::XMFLOAT3 radiance;
			float placeholder;
		};

		struct PointLightRef
		{
			DirectX::XMFLOAT3 radiance;
			uint32_t transformId;
			float radius;
		};

	public:
		static void Init();

		static void Fini();

		static LightSystem& Get() { return *s_Instance; }

		void AddPointLight(const PointLight& light);

		void Bind();

	private:
		uint32_t m_NumLights = 0;
		std::array<PointLightRef, MAX_POINT_LIGHTS> m_PointLightRefs;
	private:
		static LightSystem* s_Instance;
	};
}