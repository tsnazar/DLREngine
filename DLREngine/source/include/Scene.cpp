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
	static XMVECTOR findMaxComponent(const XMVECTOR& vec)
	{
		return XMVectorReplicate((std::max)((std::max)(XMVectorGetX(vec), XMVectorGetY(vec)), XMVectorGetZ(vec)));
	}

	bool Scene::Render(MainWindow& win, Camera& camera)
	{
		Globals::Get().Bind();

		ShaderManager::Get().GetShader("shader").SetShaders();
		TextureManager::Get().GetTexture("container").BindToPS(0);
		m_Buffer.SetBuffer();
		s_Devcon->Draw(m_Buffer.GetVertexCount(), 0);

		m_Sky.Render(camera);

		return true;
	}
}