#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>

namespace engine
{
	struct PerFrame
	{
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4 frustumCorners[3];
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

		void CreateDepthBuffer(uint32_t width, uint32_t height);
		
		void Bind(DxResPtr<ID3D11RenderTargetView>& renderTarget);
		
		void UpdateConstants();

		void SetCurrentSampler(int sampler);

		ConstantBuffer& GetPerFrameBuffer() { return m_PerFrameBuffer; }
		
		PerFrame& GetPerFrameObj() { return m_PerFrame; }

		DxResPtr<ID3D11DepthStencilView>& GetDepthBuffer() { return m_Depthbuffer; }

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

		int m_CurrentSampler = 3;

		DxResPtr<ID3D11DepthStencilView> m_Depthbuffer;
		DxResPtr<ID3D11DepthStencilState> m_DepthState;

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