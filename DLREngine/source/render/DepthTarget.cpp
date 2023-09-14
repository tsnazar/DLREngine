#include "DepthTarget.h"

namespace engine
{
	DepthTarget& DepthTarget::CreateFromDescription(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SHADER_RESOURCE_VIEW_DESC* viewDesc, const D3D11_DEPTH_STENCIL_VIEW_DESC* depthDesc)
	{
		ALWAYS_ASSERT((desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) == D3D11_BIND_DEPTH_STENCIL);
		Texture2D::CreateFromDescription(desc, viewDesc);

		HRESULT result = s_Device->CreateDepthStencilView(m_Texture.ptr(), depthDesc, m_DepthTarget.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		if (depthDesc != nullptr)
		{
			m_HasDSVDesc = true;
			m_DSVDesc = *depthDesc;
		}

		return *this;
	}

	DepthTarget& DepthTarget::RecreateDSV(const D3D11_DEPTH_STENCIL_VIEW_DESC& depthDesc)
	{
		HRESULT result = s_Device->CreateDepthStencilView(m_Texture.ptr(), &depthDesc, m_DepthTarget.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		return *this;
	}

	void DepthTarget::CopyDepthTarget(DepthTarget& target)
	{
		ALWAYS_ASSERT(s_Device != nullptr);
		ALWAYS_ASSERT(s_Devcon != nullptr);

		Texture2D::CopyTexture(target);

		if (target.GetDSVDesc() != nullptr)
		{
			m_HasDSVDesc = true;
			m_DSVDesc = *target.GetDSVDesc();

			HRESULT result = s_Device->CreateDepthStencilView(m_Texture.ptr(), &m_DSVDesc, m_DepthTarget.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}
	}

	void DepthTarget::Release()
	{
		m_Desc = { 0 };
		m_DepthTarget.release();
		m_TextureView.release();
		m_Texture.release();
	}
}