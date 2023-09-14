#include "ParticleSystem.h"
#include "Debug.h"
#include "MeshSystem.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "Globals.h"

namespace engine
{
	ParticleSystem* ParticleSystem::s_Instance = nullptr;

	const float ParticleSystem::GPU_LIFETIME = 10.0f;
	const float ParticleSystem::EDGE_SIZE = 0.1;

	ParticleSystem::ParticleSystem()
	{
		m_GpuParticles.Create<GpuParticle>(D3D11_USAGE_DEFAULT, nullptr, GPU_PARTICLES_COUNT, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

		int range[3] = { 0,0,0 };
		m_Range.Create<int>(D3D11_USAGE_DEFAULT, range, 3, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

		uint32_t args[4] = { 4,0,0,0 };
		m_IndirectArgs.Create<uint32_t>(D3D11_USAGE_DEFAULT, args, 4, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS, DXGI_FORMAT_R32_UINT);

		GpuParticleConstants constants = { GPU_PARTICLES_COUNT, GPU_LIFETIME, EDGE_SIZE, 0 };
		m_GpuConstants.Create<GpuParticleConstants>(D3D11_USAGE_DEFAULT, &constants, 1);
	}

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

	void ParticleSystem::UpdateGpuParticles(Texture2D& depth, Texture2D& normals, ConstantBuffer& dimensions)
	{
		ALWAYS_ASSERT(m_UpdateGpuParticlesShader != nullptr && m_UpdateGpuRangeShader != nullptr);

		m_GpuParticles.BindToCS(ShaderDescription::Bindings::GPU_PARTICLES_UAV);
		m_Range.BindToCS(ShaderDescription::Bindings::RANGE_UAV);
		m_IndirectArgs.BindToCS(ShaderDescription::Bindings::INDIRECT_ARGS_UAV);
		depth.BindToCS(ShaderDescription::Bindings::GBUFFER_DEPTH);
		normals.BindToCS(ShaderDescription::Bindings::GBUFFER_NORMALS);

		m_GpuConstants.BindToCS(ShaderDescription::Bindings::GPU_CONSTANTS);
		dimensions.BindToCS(ShaderDescription::Bindings::GBUFFER_DIMENSIONS);

		m_UpdateGpuParticlesShader->SetShaders();
		s_Devcon->Dispatch(std::ceilf(GPU_PARTICLES_COUNT / 64.f), 1, 1);
		m_UpdateGpuRangeShader->SetShaders();
		s_Devcon->Dispatch(1, 1, 1);
	}

	void ParticleSystem::RenderGpuParticles()
	{
		ALWAYS_ASSERT(m_ForwardGpuParticlesShader != nullptr && m_GpuNormals != nullptr);

		m_ForwardGpuParticlesShader->SetShaders();

		Globals::Get().SetReversedDepthStateReadOnly();
		Globals::Get().SetDefaultRasterizerState();
		Globals::Get().SetBlendState();

		m_GpuParticles.BindToVS(ShaderDescription::Bindings::GPU_PARTICLES_UAV);
		m_Range.BindToVS(ShaderDescription::Bindings::RANGE_UAV);
		m_GpuConstants.BindToVS(ShaderDescription::Bindings::GPU_CONSTANTS);
		m_GpuConstants.BindToPS(ShaderDescription::Bindings::GPU_CONSTANTS);
		m_GpuNormals->BindToPS(ShaderDescription::Bindings::NORMALS_PARTICLES);

		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		s_Devcon->DrawInstancedIndirect(m_IndirectArgs.GetBuffer().ptr(), 0);
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