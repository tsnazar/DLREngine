#include "Sky.h"
#include "Globals.h"

namespace engine
{
	Sky::Sky()
	{
		
	}

	void Sky::SetSky(const std::string& name, const std::string& shaderPath, const std::string& texturePath, const char* irradiancePath, const char* reflectionPath, const char* reflectancePath)
	{
		m_Shader = &ShaderManager::Get().LoadShader(shaderPath, shaderPath, nullptr);
		m_Texture = &TextureManager::Get().LoadCubemap(texturePath, texturePath);

		if (irradiancePath != nullptr && reflectionPath != nullptr && reflectancePath != nullptr)
		{
			m_hasIBLTextures = true;
			m_Irradiance = &TextureManager::Get().LoadCubemap(std::string(irradiancePath), std::string(irradiancePath));
			m_Reflection = &TextureManager::Get().LoadCubemap(std::string(reflectionPath), std::string(reflectionPath));
			m_Reflectance = &TextureManager::Get().LoadTexture2D(std::string(reflectancePath), std::string(reflectancePath));
		}
	}

	void Sky::Render(Camera& camera)
	{
		Globals::Get().SetDepthStencilStateRead(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetDefaultRasterizerState();
		Globals::Get().SetDefaultBlendState();

		m_Shader->SetShaders();
		m_Texture->BindToPS(ShaderDescription::Bindings::CUBEMAP_TEXTURE);
		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_Devcon->Draw(3, 0);
	}
}
