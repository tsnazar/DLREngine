#pragma once
#include "ConstantBuffer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Camera.h"

namespace engine
{
	class Sky
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t { CUBEMAP_TEXTURE = 0 };
		};

		Sky();

		void SetSky(const std::string& name, const std::string& shaderPath, const std::string& texturePath);
		
		void Render(Camera& camera);

	private:
		Shader* m_Shader;
		Texture2D* m_Texture;
	};
}
