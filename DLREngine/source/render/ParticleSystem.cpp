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
	
	void ParticleSystem::Render(Sky::IblResources iblResources)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(iblResources.hasResources);

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMatricesBuffer().BindToPS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);
		LightSystem::Get().GetShadowMapDimensions().BindToPS(ShaderDescription::Bindings::SHADOWMAP_DIMENSIONS);

		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);

		ShaderManager::Get().GetShader("particles").SetShaders();

		TextureManager::Get().GetTexture("smokeEMVA").BindToPS(ShaderDescription::Bindings::SMOKE_TEXTURE);
		TextureManager::Get().GetTexture("smokeRLT").BindToPS(ShaderDescription::Bindings::LIGHTMAP1_TEXTURE);
		TextureManager::Get().GetTexture("smokeBotBF").BindToPS(ShaderDescription::Bindings::LIGHTMAP2_TEXTURE);
		m_DepthCopy.BindToPS(ShaderDescription::Bindings::DEPTH_TEXTURE);

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
		MeshSystem::Get().GetLightInstances().AddInstance(&ModelManager::Get().GetModel("Sphere"), smoke.tint, smoke.transformId);
	}

	void ParticleSystem::CreateAndResolveDepthCopy()
	{
		D3D11_TEXTURE2D_DESC textureDesc = *Globals::Get().GetDepthBuffer().GetDesc();
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = *Globals::Get().GetDepthBuffer().GetSRVDesc();
		D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = *Globals::Get().GetDepthBuffer().GetDSVDesc();

		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;

		m_DepthCopy.CreateFromDescription(textureDesc, &SRVDesc, &DSVDesc);

		ID3D11RenderTargetView* const pRTV[1] = { NULL };

		s_Devcon->OMSetRenderTargets(1, pRTV, m_DepthCopy.GetDepthView().ptr());
		s_Devcon->ClearDepthStencilView(m_DepthCopy.GetDepthView().ptr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

		Globals::Get().GetDepthBuffer().BindToPS(0);

		ShaderManager::Get().GetShader("resolveDepth").SetShaders();

		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_Devcon->Draw(3, 0);

		ID3D11ShaderResourceView* const pSRV[1] = { NULL };
		s_Devcon->PSSetShaderResources(0, 1, pSRV);
	}
}