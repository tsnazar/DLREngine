#include "Postprocess.h"
#include "ShaderManager.h"
#include "Globals.h"

namespace engine
{
	Postprocess* Postprocess::s_Instance = nullptr;

	Postprocess::Postprocess()
	{
		m_ResolveConstants.Create<ResolveConstants>(D3D11_USAGE_DYNAMIC, nullptr, 1);
	}

	void Postprocess::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);

		s_Instance = new Postprocess;
	}

	void Postprocess::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);

		delete s_Instance;

		s_Instance = nullptr;
	}

	void Postprocess::Update(const ResolveConstants& constants)
	{
		m_ResolveConstants.Update(&constants, 1);
	}

	void Postprocess::Resolve(Texture2D& hdrInput, RenderTarget& ldrOutput)
	{
		const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };


		Globals::Get().Update();
		s_Devcon->OMSetRenderTargets(1, ldrOutput.GetRenderTarget().ptrAdr(), nullptr);
		s_Devcon->ClearRenderTargetView(ldrOutput.GetRenderTarget().ptr(), color);

		ShaderManager::Get().GetShader("resolve").SetShaders();
		hdrInput.BindToPS(ShaderDescription::HDR_TEXTURE);
		m_ResolveConstants.BindToPS(ShaderDescription::CONSTANTS);
		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_Devcon->Draw(3, 0);
	}
}