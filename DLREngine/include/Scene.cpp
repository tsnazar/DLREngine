#include "Scene.h"

#include <memory>
#include <limits>
#include <algorithm>
#include "ResourceManager.h"

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
		FrustumCorners f;
		DirectX::XMVECTOR TL, TR, BL, xDir, yDir;
		TL = camera.Unproject(DirectX::XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f)) - camera.Position();
		TR = camera.Unproject(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)) - camera.Position();
		BL = camera.Unproject(DirectX::XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f)) - camera.Position();
		xDir = TR - TL;
		yDir = BL - TL;
		DirectX::XMStoreFloat4(&(f.pos[0]), TL);
		DirectX::XMStoreFloat4(&(f.pos[1]), xDir);
		DirectX::XMStoreFloat4(&(f.pos[2]), yDir);

		m_ConstantBuffer.Update<FrustumCorners>(&f, sizeof(FrustumCorners));
		
		Globals::Get().BindConstantsToVS();
		Globals::Get().BindSamplerToPS();

		ResourceManager::Get().GetShader("skybox").SetShaders();
		ResourceManager::Get().GetTexture("skybox").BindToPS(0);
		m_ConstantBuffer.BindToPS(0);
		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_Devcon->IASetInputLayout(NULL);
		s_Devcon->Draw(3, 0);

		ResourceManager::Get().GetShader("shader").SetShaders();
		ResourceManager::Get().GetTexture("container").BindToPS(0);
		m_Buffer.SetBuffer();
		s_Devcon->Draw(m_Buffer.GetVertexCount(), 0);

		return true;
	}
}