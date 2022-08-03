#include "Sky.h"

namespace engine
{
	Sky::Sky()
	{
		
	}

	void Sky::SetSky(const std::string& name, const std::string& shaderPath, const std::string& texturePath)
	{
		m_Shader = &ShaderManager::Get().LoadShader(name, shaderPath, nullptr);
		m_Texture = &TextureManager::Get().LoadCubemap(name, texturePath);
	}

	void Sky::Render(Camera& camera)
	{
		m_Shader->SetShaders();
		m_Texture->BindToPS(ShaderDescription::Bindings::CUBEMAP_TEXTURE);
		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_Devcon->Draw(3, 0);
	}
}
