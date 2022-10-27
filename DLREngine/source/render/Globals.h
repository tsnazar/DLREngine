#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>
#include "LightSystem.h"
#include "DepthTarget.h"

namespace engine
{
	struct PerFrame
	{
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 invView;
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMFLOAT4X4 invProj;
		DirectX::XMFLOAT4 cameraPos;
		DirectX::XMFLOAT4 frustumCorners[3];
		float time = 0.f;
		DirectX::XMFLOAT3 padding;
		LightSystem::GpuPointLight pointLights[LightSystem::MAX_POINT_LIGHTS];
	};

	class Globals
	{
	public:
		static void Init();

		static void Fini();

		static Globals& Get() { return *s_Instance; }

		void InitD3D();

		void InitConstants();

		void InitStates();

		void Update();

		void UpdateConstants();

		void SetCurrentSampler(int sampler);

		ConstantBuffer& GetPerFrameBuffer() { return m_PerFrameBuffer; }
		
		PerFrame& GetPerFrameObj() { return m_PerFrame; }

		void SetReversedDepthState() { m_Devcon->OMSetDepthStencilState(m_DepthStateReversed.ptr(), 0); }

		void SetReversedDepthStateReadOnly() { m_Devcon->OMSetDepthStencilState(m_DepthStateReversedReadOnly.ptr(), 0); }

		void SetDepthStencilStateWrite(uint32_t stencilRef) { m_Devcon->OMSetDepthStencilState(m_DepthStencilStateWrite.ptr(), stencilRef); }

		void SetDepthStencilStateWriteIgnoreDepth(uint32_t stencilRef) { m_Devcon->OMSetDepthStencilState(m_DepthStencilStateWriteIgnoreDepth.ptr(), stencilRef); }

		void SetDepthStencilStateRead(uint32_t stencilRef) { m_Devcon->OMSetDepthStencilState(m_DepthStencilStateRead.ptr(), stencilRef); }

		void SetBlendState() { m_Devcon->OMSetBlendState(m_BlendState.ptr(), NULL, 0xffffffff); }

		void SetBlendStateAddition() { m_Devcon->OMSetBlendState(m_BlendStateAddition.ptr(), NULL, 0xffffffff); }

		void SetAlphaToCoverageBlendState() { m_Devcon->OMSetBlendState(m_AlphaToCoverageBlendState.ptr(), NULL, 0xffffffff); }

		void SetDefaultBlendState() { m_Devcon->OMSetBlendState(NULL, NULL, 0xffffffff); }

		void SetRasterizerStateCullOff() { m_Devcon->RSSetState(m_RasterizerStateCullingOff.ptr());}

		void SetRasterizerStateFrontFaceCullDepthClipOff() { m_Devcon->RSSetState(m_RasterizerStateFrontFaceCullDepthClipOff.ptr());}

		void SetRasterizerFrontFaceCull() { m_Devcon->RSSetState(m_RasterizerStateFrontFaceCull.ptr());}

		void SetDefaultRasterizerState() { m_Devcon->RSSetState(m_RasterizerState.ptr()); }

		void ResetRenderTargets() { ID3D11RenderTargetView* pRTVs[8] = { nullptr,nullptr ,nullptr ,nullptr, nullptr ,nullptr ,nullptr, nullptr }; m_Devcon->OMSetRenderTargets(8, pRTVs, NULL); }

	private:
		DxResPtr<IDXGIFactory> m_Factory;
		DxResPtr<IDXGIFactory5> m_Factory5;
		DxResPtr<ID3D11Device> m_Device;
		DxResPtr<ID3D11Device5> m_Device5;
		DxResPtr<ID3D11DeviceContext> m_Devcon;
		DxResPtr<ID3D11DeviceContext4> m_Devcon4;
		DxResPtr<ID3D11Debug> m_Devdebug;

		DxResPtr<ID3D11SamplerState> m_SamplerStatePoint;
		DxResPtr<ID3D11SamplerState> m_SamplerStateLinearMipPoint;
		DxResPtr<ID3D11SamplerState> m_SamplerStateLinear;
		DxResPtr<ID3D11SamplerState> m_SamplerStateAnisotropic;
		DxResPtr<ID3D11SamplerState> m_SamplerCmp;
		DxResPtr<ID3D11SamplerState> m_SamplerStateGrass;

		DxResPtr<ID3D11BlendState> m_BlendState;
		DxResPtr<ID3D11BlendState> m_BlendStateAddition;
		DxResPtr<ID3D11BlendState> m_AlphaToCoverageBlendState;

		DxResPtr<ID3D11RasterizerState> m_RasterizerState;
		DxResPtr<ID3D11RasterizerState> m_RasterizerStateCullingOff;
		DxResPtr<ID3D11RasterizerState> m_RasterizerStateFrontFaceCull;
		DxResPtr<ID3D11RasterizerState> m_RasterizerStateFrontFaceCullDepthClipOff;

		int m_CurrentSampler = 3;

		DxResPtr<ID3D11DepthStencilState> m_DepthStateReversed;
		DxResPtr<ID3D11DepthStencilState> m_DepthStateReversedReadOnly;
		DxResPtr<ID3D11DepthStencilState> m_DepthStencilStateWrite;
		DxResPtr<ID3D11DepthStencilState> m_DepthStencilStateWriteIgnoreDepth;
		DxResPtr<ID3D11DepthStencilState> m_DepthStencilStateRead;

		PerFrame m_PerFrame;
		ConstantBuffer m_PerFrameBuffer;
	protected:
		Globals();

		Globals(const Globals&) = delete;
		Globals& operator=(const Globals&) = delete;
		Globals(Globals&&) = delete;
		Globals& operator=(Globals&&) = delete;

		static Globals* s_Instance;
	};
}