#include "RenderTarget.h"

namespace engine
{
	RenderTarget& RenderTarget::CreateFromDescription(const D3D11_TEXTURE2D_DESC& desc)
	{
		ALWAYS_ASSERT((desc.BindFlags & D3D11_BIND_RENDER_TARGET) == D3D11_BIND_RENDER_TARGET);
		Texture2D::CreateFromDescription(desc);
		
		HRESULT result = s_Device->CreateRenderTargetView(m_Texture.ptr(), NULL, m_RenderTarget.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		return *this;
	}

	RenderTarget& RenderTarget::GetBackbufferFromSwapchain(IDXGISwapChain* swapchain)
	{
		m_TextureView.reset();

		ID3D11Texture2D* pTextureInterface = nullptr;
		HRESULT result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pTextureInterface);
		ALWAYS_ASSERT(SUCCEEDED(result));

		result = s_Device->CreateRenderTargetView(pTextureInterface, NULL, m_RenderTarget.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		*m_Texture.reset() = pTextureInterface;

		ZeroMemory(&m_Desc, sizeof(D3D11_TEXTURE2D_DESC));
		m_Texture->GetDesc(&m_Desc);

		return *this;
	}
	void RenderTarget::Release()
	{
		m_Desc = { 0 };
		m_RenderTarget.release();
		m_TextureView.release();
		m_Texture.release();
	}
}