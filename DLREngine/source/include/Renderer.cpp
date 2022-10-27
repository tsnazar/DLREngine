#include "Renderer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshSystem.h"
#include "Application.h"
#include "Postprocess.h"
#include "ParticleSystem.h"
#include "VegetationSystem.h"
#include "DecalSystem.h"

namespace engine
{
	void Renderer::CreateHDRTexture(uint32_t width, uint32_t height)
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		m_HDRTarget.CreateFromDescription(desc);
	}

	void Renderer::CreateGBuffer(uint32_t width, uint32_t height)
	{
		// create texture desc and fill it for depth buffer
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		m_GBuffer.depth.CreateFromDescription(desc, &shaderResourceViewDesc, &depthStencilViewDesc);

		// change binding type to render target
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_GBuffer.albedo.CreateFromDescription(desc);

		desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
		m_GBuffer.normals.CreateFromDescription(desc);

		desc.Format = DXGI_FORMAT_R8G8_UNORM;
		m_GBuffer.roughnessMetalness.CreateFromDescription(desc);

		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		m_GBuffer.emmission.CreateFromDescription(desc);

		desc.Format = DXGI_FORMAT_R16_UINT;
		m_GBuffer.meshID.CreateFromDescription(desc);

		GBuffer::Dimensions dims = { width, height, {0,0} };
		m_GBuffer.dimensions.Create<GBuffer::Dimensions>(D3D11_USAGE_DEFAULT, &dims, 1);
	}

	bool Renderer::Render(MainWindow& win, Camera& camera)
	{
		Globals::Get().Update();

		Globals::Get().SetReversedDepthState();
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetDefaultRasterizerState();
		LightSystem::Get().RenderToShadowMaps();

		DeferredShading(win, camera);

		Globals::Get().SetDepthStencilStateRead(0);
		Globals::Get().SetDefaultRasterizerState();
		Globals::Get().SetDefaultBlendState();
		m_Sky.Render(camera);

		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetDefaultRasterizerState();
		Globals::Get().SetReversedDepthState();
		MeshSystem::Get().GetLightInstances().Render();

		Globals::Get().SetReversedDepthStateReadOnly();
		Globals::Get().SetDefaultRasterizerState();
		Globals::Get().SetBlendState();
		ParticleSystem::Get().Render(m_Sky.GetIBLResources());

		Globals::Get().SetDefaultBlendState();
		Postprocess::Get().Resolve(m_HDRTarget, win.GetBackBuffer());

		ID3D11ShaderResourceView* const pSRV[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		s_Devcon->PSSetShaderResources(0, 8, pSRV);

		return true;
	}

	void Renderer::DeferredShading(MainWindow& win, Camera& camera)
	{
		const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		

		ID3D11RenderTargetView* pRTVs[] = { m_GBuffer.albedo.GetRenderTarget().ptr(),
													m_GBuffer.normals.GetRenderTarget().ptr(),
													m_GBuffer.roughnessMetalness.GetRenderTarget().ptr() ,
													m_GBuffer.emmission.GetRenderTarget().ptr(),
													m_GBuffer.meshID.GetRenderTarget().ptr() };
		s_Devcon->RSSetViewports(1, &win.GetViewport());
		s_Devcon->OMSetRenderTargets(5, pRTVs, m_GBuffer.depth.GetDepthView().ptr());
		s_Devcon->ClearDepthStencilView(m_GBuffer.depth.GetDepthView().ptr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
		s_Devcon->ClearRenderTargetView(m_GBuffer.albedo.GetRenderTarget().ptr(), color);
		s_Devcon->ClearRenderTargetView(m_GBuffer.normals.GetRenderTarget().ptr(), color);
		s_Devcon->ClearRenderTargetView(m_GBuffer.roughnessMetalness.GetRenderTarget().ptr(), color);
		s_Devcon->ClearRenderTargetView(m_GBuffer.emmission.GetRenderTarget().ptr(), color);
		s_Devcon->ClearRenderTargetView(m_GBuffer.meshID.GetRenderTarget().ptr(), color);

		MeshSystem::Get().RenderToGBuffer();

		Globals::Get().ResetRenderTargets();
		m_GBuffer.depthCopy.CopyTexture(m_GBuffer.depth);
		m_GBuffer.normalsCopy.CopyTexture(m_GBuffer.normals);

		s_Devcon->OMSetRenderTargets(4, pRTVs, m_GBuffer.depth.GetDepthView().ptr());

		DecalSystem::Get().RenderToGBuffer(m_GBuffer.depthCopy, m_GBuffer.dimensions, m_GBuffer.normalsCopy, m_GBuffer.meshID);

		VegetationSystem::Get().RenderToGBuffer();

		Globals::Get().ResetRenderTargets();
		m_GBuffer.depthCopy.CopyTexture(m_GBuffer.depth);

		s_Devcon->RSSetViewports(1, &win.GetViewport());
		s_Devcon->OMSetRenderTargets(1, m_HDRTarget.GetRenderTarget().ptrAdr(), m_GBuffer.depth.GetDepthView().ptr());
		s_Devcon->ClearRenderTargetView(m_HDRTarget.GetRenderTarget().ptr(), color);
		
		MeshSystem::Get().ResolveGBuffer(m_Sky.GetIBLResources(), m_GBuffer.depthCopy, m_GBuffer.albedo, m_GBuffer.normals, m_GBuffer.roughnessMetalness,
			m_GBuffer.emmission, m_GBuffer.dimensions);
		
		VegetationSystem::Get().ResolveGBuffer(m_Sky.GetIBLResources(), m_GBuffer.depthCopy, m_GBuffer.albedo, m_GBuffer.normals, m_GBuffer.roughnessMetalness,
			m_GBuffer.emmission, m_GBuffer.dimensions);
	}

	void Renderer::Update(float dt, Camera& camera)
	{
		const Postprocess::ResolveConstants constants = { m_EV100, {0,0,0} };

		DecalSystem::Get().Update();
		Postprocess::Get().Update(constants);
		LightSystem::Get().Update();
		MeshSystem::Get().Update(dt);
		ParticleSystem::Get().Update(dt, camera);

	}
}