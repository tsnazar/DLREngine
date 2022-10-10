#include "Texture2D.h"
#include "DDSTextureLoader.h"
#include "Globals.h"

namespace engine
{
	Texture2D& Texture2D::LoadFromFile(const std::string& filepath, uint32_t miscFlags, size_t maxsize, D3D11_USAGE usage, uint32_t bindFlags, uint32_t cpuAccessFlags, bool forceSRGB)
	{
		ALWAYS_ASSERT(s_Device != nullptr);
		ALWAYS_ASSERT(s_Devcon != nullptr);

		const std::wstring stemp = std::wstring(filepath.begin(), filepath.end());

		ID3D11Resource* pResource = nullptr;
		HRESULT result = DirectX::CreateDDSTextureFromFileEx(s_Device, stemp.c_str(), maxsize, usage, bindFlags, cpuAccessFlags, miscFlags, forceSRGB, &pResource, m_TextureView.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		*m_Texture.reset() = static_cast<ID3D11Texture2D*>(pResource);

		ZeroMemory(&m_Desc, sizeof(D3D11_TEXTURE2D_DESC));
		m_Texture->GetDesc(&m_Desc);

		return *this;
	}

	Texture2D& Texture2D::CreateFromDescription(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SHADER_RESOURCE_VIEW_DESC* viewDesc)
	{
		ALWAYS_ASSERT(s_Device != nullptr);
		ALWAYS_ASSERT(s_Devcon != nullptr);

		m_Desc = desc;
		HRESULT result = s_Device->CreateTexture2D(&m_Desc, NULL, m_Texture.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		if ((m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
		{
			result = s_Device->CreateShaderResourceView(m_Texture.ptr(), viewDesc, m_TextureView.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		if (viewDesc != nullptr)
		{
			m_HasSRVDesc = true;
			m_SRVDesc = *viewDesc;
		}

		return *this;
	}

	void Texture2D::CopyTexture(Texture2D& texture)
	{
		ALWAYS_ASSERT(s_Device != nullptr);
		ALWAYS_ASSERT(s_Devcon != nullptr);

		m_Desc = *texture.GetDesc();
		HRESULT result = s_Device->CreateTexture2D(&m_Desc, NULL, m_Texture.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		if ((m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
		{
			result = s_Device->CreateShaderResourceView(m_Texture.ptr(), texture.GetSRVDesc(), m_TextureView.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		if (texture.GetSRVDesc() != nullptr)
		{
			m_HasSRVDesc = true;
			m_SRVDesc = *texture.GetSRVDesc();
		}

		s_Devcon->CopyResource(m_Texture.ptr(), texture.GetTexture().ptr());
	}

	void Texture2D::ResolveTexture(Texture2D& texture)
	{
		ALWAYS_ASSERT(s_Device != nullptr);
		ALWAYS_ASSERT(s_Devcon != nullptr);

		m_Desc = *texture.GetDesc();
		m_Desc.SampleDesc.Count = 1;
		m_Desc.SampleDesc.Quality = 0;
		m_Desc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT result = s_Device->CreateTexture2D(&m_Desc, NULL, m_Texture.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		if ((m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE)
		{
			m_Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			result = s_Device->CreateShaderResourceView(m_Texture.ptr(), texture.GetSRVDesc(), m_TextureView.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		if (texture.GetSRVDesc() != nullptr)
		{
			m_HasSRVDesc = true;
			m_SRVDesc = *texture.GetSRVDesc();
		}

		s_Devcon->ResolveSubresource(m_Texture.ptr(), 0, texture.GetTexture().ptr(), 0, m_SRVDesc.Format);
	}

	void Texture2D::BindToVS(uint32_t slot)
	{
		ALWAYS_ASSERT(s_Devcon != nullptr && m_TextureView.valid());
		s_Devcon->VSSetShaderResources(slot, 1, m_TextureView.ptrAdr());
	}

	void Texture2D::BindToGS(uint32_t slot)
	{
		ALWAYS_ASSERT(s_Devcon != nullptr && m_TextureView.valid());
		s_Devcon->GSSetShaderResources(slot, 1, m_TextureView.ptrAdr());
	}

	void Texture2D::BindToPS(uint32_t slot)
	{
		ALWAYS_ASSERT(s_Devcon != nullptr && m_TextureView.valid());
		s_Devcon->PSSetShaderResources(slot, 1, m_TextureView.ptrAdr());
	}

}