#include "Globals.h"
#include <utility>
#include <iostream>

extern "C"
{
	_declspec(dllexport) uint32_t NvOptimusEnablement = 1;
	_declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

#define ALWAYS_ASSERT(expr)\
	if(!expr)\
		__debugbreak()\

namespace engine
{

	void Globals::InitD3D()
	{
		HRESULT result;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)m_Factory.reset());
		//ALWAYS_ASSERT(result >= 0);

		result = m_Factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)m_Factory5.reset());
		//ALWAYS_ASSERT(result >= 0);

		{
			uint32_t index = 0;

			IDXGIAdapter1* adapter;
			while (m_Factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				std::cout << "GPU #" << index << desc.Description << std::endl;
			}
		}

		const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
		result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
			&featureLevelRequested, 1, D3D11_SDK_VERSION, m_Device.reset(), &featureLevelInitialized, m_Devcon.reset());
		//ALWAYS_ASSERT(result >= 0);
		//ALWAYS_ASSERT(featureLevelRequested == featureLevelInitialized);

		result = m_Device->QueryInterface(__uuidof(ID3D11Device5), (void**)m_Device5.reset());
		//ALWAYS_ASSERT(result >= 0);

		result = m_Devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_Devcon4.reset());
		//ALWAYS_ASSERT(result >= 0);

		result = m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)m_Devdebug.reset());
		//ALWAYS_ASSERT(result >= 0);


		s_Factory = m_Factory5.ptr();
		s_Device = m_Device5.ptr();
		s_Devcon = m_Devcon4.ptr();
	}
}