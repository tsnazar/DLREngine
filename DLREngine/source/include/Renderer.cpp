#include "Renderer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshSystem.h"

namespace engine
{
	bool Renderer::Render(MainWindow& win, Camera& camera)
	{

		ShaderManager::Get().GetShader("instance").SetShaders();
		ShaderManager::Get().GetInputLayout({VertexType::PosTex, InstanceType::Transform}).SetInputLayout();
		MeshSystem::Get().Render();

		m_Sky.Render(camera);

		return true;
	}
}