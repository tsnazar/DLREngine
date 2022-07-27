#include "Scene.h"

#include <memory>
#include <limits>
#include <algorithm>
#include "TextureManager.h"
#include "ShaderManager.h"

using namespace DirectX;

namespace
{
	const float SHADOW_BIAS = 0.0005f;
	const float MIRROR_BIAS = 0.0005f;
	const float LIGHT_REP_RADIUS = 0.1f;
	const float SCENE_REFLECTION_MAX_ROUGHNESS = 0.1f;
	const int MAX_DEPTH = 20;
	const XMVECTOR GAMMA_CORRECTION = XMVectorReplicate(1.0f / 2.2f);
}

namespace engine
{
	bool Scene::Render(MainWindow& win, Camera& camera)
	{

		ShaderManager::Get().GetShader("instance").SetShaders();
		ShaderManager::Get().GetInputLayout({VertexType::PosTex, InstanceType::Transform}).SetInputLayout();
		m_Opaque.Render();

		m_Sky.Render(camera);

		return true;
	}
	void Scene::Update(float dt)
	{
		m_Opaque.UpdateInstanceBuffers();
	}
}