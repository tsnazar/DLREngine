#include "VegetationSystem.h"
#include "PoissonDiskSampling.h"
#include <iostream>
#include "TextureManager.h"
#include "LightSystem.h"

namespace engine
{
	VegetationSystem* VegetationSystem::s_Instance = nullptr;

	void VegetationSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);

		s_Instance = new VegetationSystem;
	}

	void VegetationSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);

		delete s_Instance;

		s_Instance = nullptr;
	}

	void VegetationSystem::CreateField(const DirectX::XMFLOAT3& pos, float length, float radius, float minSize, float maxSize, float windAngle)
	{
		m_Center = pos;
		m_Length = length;
		m_Radius = radius;

		Wind wind;

		float windCos = cosf(windAngle);
		float windSin = sinf(windAngle);

		wind.rot[0] = { windCos, windSin };
		wind.rot[1] = { -windSin, windCos };

		wind.invRot[0] = { windCos, -windSin };
		wind.invRot[1] = { windSin, windCos };

		m_WindBuffer.Create<Wind>(D3D11_USAGE_DEFAULT, &wind, 1);

		std::vector<DirectX::XMFLOAT2> points;
		poissonDiskSampling(m_Length, m_Length, radius, 30, points);

		std::vector<Instance> instances;
		Instance instance;

		//instance.scale = 1.0f;
		//instance.pos = { m_Center.x, m_Center.y, m_Center.z };
		DirectX::XMFLOAT3 TL = { m_Center.x - m_Length / 2.f, m_Center.y, m_Center.z - m_Length / 2.f };

		//instances.push_back(instance);
		for (const auto& point : points)
		{
			instance.scale = minSize + rand() / (RAND_MAX /(maxSize - minSize));
			//instance.scale = 1.0f;
			instance.pos = { TL.x + point.x, TL.y - (instance.scale / 8.f), TL.z + point.y };
			float angle = ((rand() % 360) * DirectX::XM_PI) / 180.f;
			float cos = cosf(angle);
			float sin = sinf(angle);
			instance.rot[0] = { cos, sin };
			instance.rot[1] = { -sin, cos };
			instances.push_back(instance);
		}

		m_InstanceBuffer.Create<Instance>(D3D11_USAGE_IMMUTABLE, instances.data(), instances.size());
	}

	void VegetationSystem::Render(Sky::IblResources iblResources)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		m_WindBuffer.BindToGS(ShaderDescription::Bindings::WIND_BUFFER);

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMatricesBuffer().BindToPS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);
		LightSystem::Get().GetShadowMapDimensions().BindToPS(ShaderDescription::Bindings::SHADOWMAP_DIMENSIONS);

		ShaderManager::Get().GetShader("grass").SetShaders();
		TextureManager::Get().GetTexture("grassAlbedo").BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);
		TextureManager::Get().GetTexture("grassRoughness").BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);
		TextureManager::Get().GetTexture("grassMetallic").BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);
		TextureManager::Get().GetTexture("grassNormal").BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);
		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);
		TextureManager::Get().GetTexture("grassOpacity").BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);
		TextureManager::Get().GetTexture("grassAO").BindToPS(ShaderDescription::Bindings::AO_TEXTURE);
		TextureManager::Get().GetTexture("grassTranslucency").BindToPS(ShaderDescription::Bindings::TRANSLUCENCY_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		s_Devcon->DrawInstanced(8, m_InstanceBuffer.GetVertexCount(), 0, 0);
	}

	void VegetationSystem::RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		TextureManager::Get().GetTexture("grassOpacity").BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);

		ShaderManager::Get().GetShader("shadowsGrass").SetShaders();

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		m_WindBuffer.BindToGS(4);

		LightSystem::ShadowMapGeometryShaderConstants con;

		for (uint32_t i = 0; i < numLights; ++i)
		{
			memcpy(&con.matrices, &matrices[i], sizeof(DirectX::XMFLOAT4X4[6]));
			con.sliceOffset = i * 6;
			shadowMatrixBuffer.Update(&con, 1);
			shadowMatrixBuffer.BindToGS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);

			s_Devcon->DrawInstanced(8, m_InstanceBuffer.GetVertexCount(), 0, 0);
		}
	}
}

