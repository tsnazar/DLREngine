#include "Renderer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshSystem.h"
#include "Application.h"
#include "Postprocess.h"

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
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		m_HDRTarget.CreateFromDescription(desc);
	}

	bool Renderer::Render(MainWindow& win, Camera& camera)
	{
		const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

		Globals::Get().Update();

		Globals::Get().SetDepthState();
		LightSystem::Get().RenderToShadowMaps();

		Globals::Get().SetReversedDepthState();
		s_Devcon->GSSetShader(nullptr, NULL, 0);
		s_Devcon->RSSetViewports(1, &win.GetViewport());
		s_Devcon->OMSetRenderTargets(1, m_HDRTarget.GetRenderTarget().ptrAdr(), Globals::Get().GetDepthBuffer().ptr());
		s_Devcon->ClearDepthStencilView(Globals::Get().GetDepthBuffer().ptr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
		s_Devcon->ClearRenderTargetView(m_HDRTarget.GetRenderTarget().ptr(), color);

		MeshSystem::Get().Render(m_Sky.GetIBLResources());
		m_Sky.Render(camera);

		Postprocess::Get().Resolve(m_HDRTarget, win.GetBackBuffer());

		ID3D11ShaderResourceView* const pSRV[1] = { NULL };
		s_Devcon->PSSetShaderResources(0, 1, pSRV);

		return true;
	}

	void Renderer::Update()
	{
		const Postprocess::ResolveConstants constants = { m_EV100, {0,0,0} };

		Postprocess::Get().Update(constants);
		LightSystem::Get().Update();
		MeshSystem::Get().Update();
	}
}