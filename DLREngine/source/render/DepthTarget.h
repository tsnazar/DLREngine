#pragma once
#include "Texture2D.h"

namespace engine
{
	class DepthTarget : public Texture2D
	{
	public:

		DepthTarget& CreateFromDescription(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SHADER_RESOURCE_VIEW_DESC* viewDesc = nullptr, const D3D11_DEPTH_STENCIL_VIEW_DESC* depthDesc = nullptr);

		DepthTarget& RecreateDSV(const D3D11_DEPTH_STENCIL_VIEW_DESC& depthDesc);

		void Release();

		DxResPtr<ID3D11DepthStencilView>& GetDepthView() { return m_DepthTarget; }
	private:
		DxResPtr<ID3D11DepthStencilView> m_DepthTarget;
	};
}