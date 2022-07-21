#include "Globals.h"
#include <utility>
#include <iostream>
#include "Debug.h"

extern "C"
{
	_declspec(dllexport) uint32_t NvOptimusEnablement = 1;
	_declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace engine
{
	Globals* Globals::s_Instance = nullptr;

	Globals::Globals()
	{

	}

	void Globals::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);

		s_Instance = new Globals();

		s_Instance->InitD3D();
		s_Instance->InitConstants();
	}

	void Globals::Fini()
	{
		//s_Device->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
		ALWAYS_ASSERT(s_Instance != nullptr);
		delete s_Instance;
		s_Instance = nullptr;
		s_Device = nullptr;
		s_Debug = nullptr;
		s_Devcon = nullptr;
		s_Factory = nullptr;
	}

	void Globals::InitD3D()
	{
		HRESULT result;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)m_Factory.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		result = m_Factory->QueryInterface(__uuidof(IDXGIFactory5), (void**)m_Factory5.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

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
		ALWAYS_ASSERT(SUCCEEDED(result));
		ALWAYS_ASSERT(featureLevelRequested == featureLevelInitialized);

		result = m_Device->QueryInterface(__uuidof(ID3D11Device5), (void**)m_Device5.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		result = m_Devcon->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_Devcon4.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		result = m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)m_Devdebug.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = m_Device->CreateSamplerState(&sampDesc, m_SamplerStatePoint.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		result = m_Device->CreateSamplerState(&sampDesc, m_SamplerStateLinearMipPoint.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		result = m_Device->CreateSamplerState(&sampDesc, m_SamplerStateLinear.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
		result = m_Device->CreateSamplerState(&sampDesc, m_SamplerStateAnisotropic.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		s_Factory = m_Factory5.ptr();
		s_Device = m_Device5.ptr();
		s_Devcon = m_Devcon4.ptr();
		s_Debug = m_Devdebug.ptr();
	}

	void Globals::InitConstants()
	{
		m_PerFrameBuffer.Create<PerFrame>(D3D11_USAGE_DYNAMIC, nullptr, 0);
	}

	void Globals::Bind()
	{
		m_PerFrameBuffer.BindToVS(0);

		if (m_CurrentSampler == 1)
			m_Devcon->PSSetSamplers(0, 1, m_SamplerStatePoint.ptrAdr());
		else if (m_CurrentSampler == 2)
			m_Devcon->PSSetSamplers(0, 1, m_SamplerStateLinearMipPoint.ptrAdr());
		else if (m_CurrentSampler == 3)
			m_Devcon->PSSetSamplers(0, 1, m_SamplerStateLinear.ptrAdr());
		else if (m_CurrentSampler == 4)
			m_Devcon->PSSetSamplers(0, 1, m_SamplerStateAnisotropic.ptrAdr());

		this->UpdateConstants();
	}

	void Globals::UpdateConstants()
	{
		m_PerFrameBuffer.Update(&m_PerFrame, sizeof(PerFrame));
	}

	void Globals::SetCurrentSampler(int sampler)
	{
		m_CurrentSampler = sampler;
	}
}