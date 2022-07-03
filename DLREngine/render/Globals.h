#pragma once
#include "d3d.h"
#include "DxRes.h"

namespace engine
{
	class Globals
	{
	public:
		void InitD3D();
	private:
		DxResPtr<IDXGIFactory> m_Factory;
		DxResPtr<IDXGIFactory5> m_Factory5;
		DxResPtr<ID3D11Device> m_Device;
		DxResPtr<ID3D11Device5> m_Device5;
		DxResPtr<ID3D11DeviceContext> m_Devcon;
		DxResPtr<ID3D11DeviceContext4> m_Devcon4;
		DxResPtr<ID3D11Debug> m_Devdebug;
	};
}