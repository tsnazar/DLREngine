#include "Renderer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshSystem.h"

namespace engine
{
	bool Renderer::Render(MainWindow& win, Camera& camera)
	{

		MeshSystem::Get().Render();

		m_Sky.Render(camera);

		return true;
	}
}