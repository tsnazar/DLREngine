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
	};

	class Globals
	{
	public:
		Globals();
		~Globals();

		Globals(const Globals&) = delete;
		Globals& operator=(const Globals&) = delete;
		Globals(Globals&&) = delete;
		Globals& operator=(Globals&&) = delete;

		static Globals& Get() { return *s_Instance; }

		void InitD3D();
		void InitConstants();

		void BindConstantsToVS();
		
		void UpdateConstants();

		void BindSamplerToPS();

		void SetCurrentSampler(int sampler);

		ConstantBuffer& GetPerFrameBuffer() { return m_PerFrameBuffer; }
		PerFrame& GetPerFrameObj() { return m_PerFrame; }

	private:
		DxResPtr<IDXGIFactory> m_Factory;
		DxResPtr<IDXGIFactory5> m_Factory5;
		DxResPtr<ID3D11Device> m_Device;
		DxResPtr<ID3D11Device5> m_Device5;
		DxResPtr<ID3D11DeviceContext> m_Devcon;
		DxResPtr<ID3D11DeviceContext4> m_Devcon4;
		DxResPtr<ID3D11Debug> m_Devdebug;
		DxResPtr<ID3D11SamplerState> m_SamplerState1;
		DxResPtr<ID3D11SamplerState> m_SamplerState2;
		DxResPtr<ID3D11SamplerState> m_SamplerState3;
		DxResPtr<ID3D11SamplerState> m_SamplerState4;

		int m_CurrentSampler = 1;

		PerFrame m_PerFrame;
		ConstantBuffer m_PerFrameBuffer;
	private:
		static Globals* s_Instance;
	};
}