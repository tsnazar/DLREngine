#include "Sky.h"

namespace engine
{
	Sky::Sky()
	{
		
	}

	void Sky::SetSky(const std::string& name, const std::string& shaderPath, const std::string& texturePath)
	{
		m_Shader = &ShaderManager::Get().LoadShader(name, { VertexType::Undefined, InstanceType::Undefined }, shaderPath);
		m_Texture = &TextureManager::Get().LoadCubemap(name, texturePath);
	}

	void Sky::Render(Camera& camera)
	{
		m_Shader->SetShaders();
		m_Texture->BindToPS(0);
		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_Devcon->IASetInputLayout(NULL);
		s_Devcon->Draw(3, 0);
	}
}
