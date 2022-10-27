#include "VegetationSystem.h"
#include "PoissonDiskSampling.h"
#include <iostream>
#include "TextureManager.h"
#include "LightSystem.h"
#include "Globals.h"
#include "ModelManager.h"

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

		DirectX::XMFLOAT3 TL = { m_Center.x - m_Length / 2.f, m_Center.y, m_Center.z - m_Length / 2.f };

		for (const auto& point : points)
		{
			instance.scale = minSize + rand() / (RAND_MAX / (maxSize - minSize));
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

		ALWAYS_ASSERT(iblResources.hasResources);

		ALWAYS_ASSERT(m_ForwardShader != nullptr && m_Textures.IsValid());
		
		m_ForwardShader->SetShaders();
		//ShaderManager::Get().GetShader("grass").SetShaders();

		m_WindBuffer.BindToVS(ShaderDescription::Bindings::WIND_BUFFER);

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMatricesBuffer().BindToPS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);
		LightSystem::Get().GetShadowMapDimensions().BindToPS(ShaderDescription::Bindings::SHADOWMAP_DIMENSIONS);

		m_Textures.albedo->BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);
		m_Textures.roughness->BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);
		m_Textures.metallic->BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);
		m_Textures.normals->BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);
		m_Textures.opacity->BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);
		m_Textures.ao->BindToPS(ShaderDescription::Bindings::AO_TEXTURE);
		m_Textures.translucency->BindToPS(ShaderDescription::Bindings::TRANSLUCENCY_TEXTURE);
		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);

		//TextureManager::Get().GetTexture("grassAlbedo").BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);
		//TextureManager::Get().GetTexture("grassRoughness").BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);
		//TextureManager::Get().GetTexture("grassMetallic").BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);
		//TextureManager::Get().GetTexture("grassNormal").BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);
		//TextureManager::Get().GetTexture("grassOpacity").BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);
		//TextureManager::Get().GetTexture("grassAO").BindToPS(ShaderDescription::Bindings::AO_TEXTURE);
		//TextureManager::Get().GetTexture("grassTranslucency").BindToPS(ShaderDescription::Bindings::TRANSLUCENCY_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		s_Devcon->DrawInstanced(72, m_InstanceBuffer.GetVertexCount(), 0, 0);
	}

	void VegetationSystem::RenderToGBuffer()
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_GBufferShader != nullptr);

		m_GBufferShader->SetShaders();

		Globals::Get().SetDepthStencilStateWrite(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetRasterizerStateCullOff();

		m_WindBuffer.BindToVS(ShaderDescription::Bindings::WIND_BUFFER);

		//ShaderManager::Get().GetShader("grassGB").SetShaders();
		m_Textures.albedo->BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);
		m_Textures.roughness->BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);
		m_Textures.metallic->BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);
		m_Textures.normals->BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);
		m_Textures.opacity->BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);
		m_Textures.ao->BindToPS(ShaderDescription::Bindings::AO_TEXTURE);
		m_Textures.translucency->BindToPS(ShaderDescription::Bindings::TRANSLUCENCY_TEXTURE);

		//TextureManager::Get().GetTexture("grassAlbedo").BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);
		//TextureManager::Get().GetTexture("grassRoughness").BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);
		//TextureManager::Get().GetTexture("grassMetallic").BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);
		//TextureManager::Get().GetTexture("grassNormal").BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);
		//TextureManager::Get().GetTexture("grassOpacity").BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);
		//TextureManager::Get().GetTexture("grassAO").BindToPS(ShaderDescription::Bindings::AO_TEXTURE);
		//TextureManager::Get().GetTexture("grassTranslucency").BindToPS(ShaderDescription::Bindings::TRANSLUCENCY_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		s_Devcon->DrawInstanced(72, m_InstanceBuffer.GetVertexCount(), 0, 0);
	}

	void VegetationSystem::ResolveGBuffer(Sky::IblResources iblResources, Texture2D& depth, Texture2D& albedo, Texture2D& normals,
		Texture2D& roughnessMetallic, Texture2D& emission, ConstantBuffer& dimensions)
	{
		auto& instanceBuffer = LightSystem::Get().GetDeferedShadingLightInstances();

		if (instanceBuffer.GetVertexCount() == 0 || !instanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(iblResources.hasResources);

		ALWAYS_ASSERT(m_DeferredShader != nullptr && m_DeferredIBLShader != nullptr);

		m_DeferredShader->SetShaders();

		//ShaderManager::Get().GetShader("grassDS").SetShaders();

		Globals::Get().SetDepthStencilStateRead(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetBlendStateAddition();
		Globals::Get().SetRasterizerStateFrontFaceCullDepthClipOff();

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMatricesBuffer().BindToPS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);
		LightSystem::Get().GetShadowMapDimensions().BindToPS(ShaderDescription::Bindings::SHADOWMAP_DIMENSIONS);

		dimensions.BindToPS(ShaderDescription::Bindings::TARGET_DIMENSIONS_CONSTANTS);

		depth.BindToPS(ShaderDescription::Bindings::DEPTH_DS_TEXTURE);
		albedo.BindToPS(ShaderDescription::Bindings::ALBEDO_DS_TEXTURE);
		normals.BindToPS(ShaderDescription::Bindings::NORMALS_DS_TEXTURE);
		roughnessMetallic.BindToPS(ShaderDescription::Bindings::ROUGHMETALLIC_DS_TEXTURE);
		emission.BindToPS(ShaderDescription::Bindings::EMISSION_DS_TEXTURE);

		Model& sphere = ModelManager::Get().GetUnitSphere();
		sphere.Bind(ShaderDescription::Bindings::MESH_DS_BUFFER);

		instanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_DS_BUFFER);

		s_Devcon->DrawInstanced(sphere.GetSubMeshes()[0].vertexNum, instanceBuffer.GetVertexCount(), 0, sphere.GetSubMeshes()[0].vertexOffset);

		m_DeferredIBLShader->SetShaders();
		//ShaderManager::Get().GetShader("grassIBLDS").SetShaders();

		Globals::Get().SetDepthStencilStateRead(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetBlendStateAddition();
		Globals::Get().SetDefaultRasterizerState();

		emission.BindToPS(ShaderDescription::Bindings::EMISSION_GB_TEXTURE);
		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);
		iblResources.reflection->BindToPS(ShaderDescription::Bindings::REFLECTION_TEXTURE);
		iblResources.reflectance->BindToPS(ShaderDescription::Bindings::REFLECTANCE_TEXTURE);

		s_Devcon->Draw(3, 0);
	}

	void VegetationSystem::RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_ShadowsShader != nullptr);

		m_ShadowsShader->SetShaders();
		//ShaderManager::Get().GetShader("grassShadows").SetShaders();

		Globals::Get().SetReversedDepthState();
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetDefaultRasterizerState();

		m_WindBuffer.BindToVS(ShaderDescription::Bindings::WIND_BUFFER);

		m_Textures.opacity->BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);
		//TextureManager::Get().GetTexture("grassOpacity").BindToPS(ShaderDescription::Bindings::OPACITY_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		LightSystem::ShadowMapGeometryShaderConstants con;

		for (uint32_t i = 0; i < numLights; ++i)
		{
			memcpy(&con.matrices, &matrices[i], sizeof(DirectX::XMFLOAT4X4[6]));
			con.sliceOffset = i * 6;
			shadowMatrixBuffer.Update(&con, 1);
			shadowMatrixBuffer.BindToGS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);

			s_Devcon->DrawInstanced(72, m_InstanceBuffer.GetVertexCount(), 0, 0);
		}
	}
}

