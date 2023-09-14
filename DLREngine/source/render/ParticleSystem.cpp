#include "ParticleSystem.h"
#include "Debug.h"
#include "MeshSystem.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "Globals.h"

namespace engine
{
	ParticleSystem* ParticleSystem::s_Instance = nullptr;

	void ParticleSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);

		s_Instance = new ParticleSystem;
	}

	void ParticleSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);

		delete s_Instance;

		s_Instance = nullptr;
	}
	
	void ParticleSystem::Render(Sky::IblResources iblResources, Texture2D& depth, ConstantBuffer& dimensions)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(iblResources.hasResources);

		ALWAYS_ASSERT(m_ForwardShader != nullptr && m_Textures.IsValid());

		m_ForwardShader->SetShaders();

		Globals::Get().SetReversedDepthStateReadOnly();
		Globals::Get().SetDefaultRasterizerState();
		Globals::Get().SetBlendState();

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMatricesBuffer().BindToPS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);
		LightSystem::Get().GetShadowMapDimensions().BindToPS(ShaderDescription::Bindings::SHADOWMAP_DIMENSIONS);

		dimensions.BindToPS(ShaderDescription::Bindings::TARGET_DIMENSIONS_CONSTANTS);

		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);

		m_Textures.EMVA->BindToPS(ShaderDescription::Bindings::SMOKE_TEXTURE);
		m_Textures.lightMapRLT->BindToPS(ShaderDescription::Bindings::LIGHTMAP1_TEXTURE);
		m_Textures.lightMapBotBF->BindToPS(ShaderDescription::Bindings::LIGHTMAP2_TEXTURE);
		depth.BindToPS(ShaderDescription::Bindings::DEPTH_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		s_Devcon->DrawInstanced(4, m_InstanceBuffer.GetVertexCount(), 0, 0);
	}
	
	void ParticleSystem::Update(float dt, Camera& camera)
	{
		std::vector<Particle> particles;
		for (auto& emmiter : m_Emmiters)
		{
			emmiter.Update(dt);
			for (auto& p : emmiter.particles)
				particles.push_back(p);
		}

		DirectX::XMFLOAT3 cameraPos;
		DirectX::XMStoreFloat3(&cameraPos, camera.Position());

		std::sort(particles.begin(), particles.end(), [&](Particle a, Particle b) {
			float d1 = Length(a.pos - cameraPos);
			float d2 = Length(b.pos - cameraPos);
			return d1 < d2;
			});

		if (particles.size() > 0)
			m_InstanceBuffer.Create<Particle>(D3D11_USAGE_DYNAMIC, particles.data(), particles.size());

	}
	
	void ParticleSystem::AddSmoke(const SmokeEmitter& smoke)
	{
		m_Emmiters.push_back(smoke);
		MeshSystem::Get().GetLightInstances().AddInstance(&ModelManager::Get().GetUnitSphere(), smoke.tint, smoke.transformId);
	}

}