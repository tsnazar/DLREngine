#pragma once
#include "Texture2D.h"

namespace engine
{
	class RenderTarget : public Texture2D
	{
	public:

		RenderTarget& CreateFromDescription(const D3D11_TEXTURE2D_DESC& desc);
		
		RenderTarget& GetBackbufferFromSwapchain(IDXGISwapChain* swapchain);

		void Release();

		DxResPtr<ID3D11RenderTargetView>& GetRenderTarget() { return m_RenderTarget; }

	private:
		DxResPtr<ID3D11RenderTargetView> m_RenderTarget;
	};
}