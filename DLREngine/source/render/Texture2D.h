#pragma once

#include "d3d.h"
#include "DxRes.h"
#include "Debug.h"
#include <string>

namespace engine
{
	class Texture2D
	{
	public:
		Texture2D() {}

		Texture2D& LoadFromFile(const std::string& filepath, uint32_t miscFlags = 0, size_t maxsize = 0, D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE,
							uint32_t bindFlags = D3D11_BIND_SHADER_RESOURCE, uint32_t cpuAccessFlags = 0, bool forceSRGB = false);

		Texture2D& CreateFromDescription(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SHADER_RESOURCE_VIEW_DESC* viewDesc = nullptr);

		D3D11_TEXTURE2D_DESC* GetDesc() { return &m_Desc; }
		D3D11_SHADER_RESOURCE_VIEW_DESC* GetSRVDesc() { return m_HasSRVDesc ? &m_SRVDesc : nullptr; }
		DxResPtr<ID3D11Texture2D>& GetTexture() { return m_Texture; }

		void CopyTexture(Texture2D& texture);
		void ResolveTexture(Texture2D& texture);

		void BindToVS(uint32_t slot);
		void BindToGS(uint32_t slot);
		void BindToPS(uint32_t slot);

	protected:
		DxResPtr<ID3D11ShaderResourceView> m_TextureView;
		DxResPtr<ID3D11Texture2D> m_Texture;
		D3D11_TEXTURE2D_DESC m_Desc;
		D3D11_SHADER_RESOURCE_VIEW_DESC m_SRVDesc;
		bool m_HasSRVDesc = false;
	};
}
