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

		void LoadFromFile(const std::string& filepath, uint32_t miscFlags = 0, size_t maxsize = 0, D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE,
							uint32_t bindFlags = D3D11_BIND_SHADER_RESOURCE, uint32_t cpuAccessFlags = 0, bool forceSRGB = false);

		void BindToVS(uint32_t slot);
		void BindToGS(uint32_t slot);
		void BindToPS(uint32_t slot);

	private:
		DxResPtr<ID3D11ShaderResourceView> m_Texture;
	};
}
