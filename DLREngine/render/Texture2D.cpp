#include "Texture2D.h"
#include "DDSTextureLoader.h"
#include "Globals.h"

namespace engine
{
	void Texture2D::LoadFromFile(const std::string& filepath, uint32_t miscFlags, size_t maxsize, D3D11_USAGE usage, uint32_t bindFlags, uint32_t cpuAccessFlags, bool forceSRGB)
	{
		ALWAYS_ASSERT(s_Device != nullptr);
		ALWAYS_ASSERT(s_Devcon != nullptr);

		const std::wstring stemp = std::wstring(filepath.begin(), filepath.end());

		HRESULT result = DirectX::CreateDDSTextureFromFileEx(s_Device, stemp.c_str(), maxsize, usage, bindFlags, cpuAccessFlags, miscFlags, forceSRGB, nullptr, m_Texture.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));
	}

	void Texture2D::BindToVS(uint32_t slot)
	{
		ALWAYS_ASSERT(s_Devcon != nullptr);
		s_Devcon->VSSetShaderResources(slot, 1, m_Texture.ptrAdr());
	}

	void Texture2D::BindToGS(uint32_t slot)
	{
		ALWAYS_ASSERT(s_Devcon != nullptr);
		s_Devcon->GSSetShaderResources(slot, 1, m_Texture.ptrAdr());
	}

	void Texture2D::BindToPS(uint32_t slot)
	{
		ALWAYS_ASSERT(s_Devcon != nullptr);
		s_Devcon->PSSetShaderResources(slot, 1, m_Texture.ptrAdr());
	}

}