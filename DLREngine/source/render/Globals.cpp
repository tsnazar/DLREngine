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
		s_Instance->InitStates();
	}

	void Globals::Fini()
	{
		//s_Debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
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

		s_Factory = m_Factory5.ptr();
		s_Device = m_Device5.ptr();
		s_Devcon = m_Devcon4.ptr();
		s_Debug = m_Devdebug.ptr();
	}

	void Globals::InitConstants()
	{
		m_PerFrameBuffer.Create<PerFrame>(D3D11_USAGE_DYNAMIC, nullptr, 1);
	}

	void Globals::InitStates()
	{
		//Create Depth state
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

		HRESULT result = m_Device5->CreateDepthStencilState(&depthStencilDesc, m_DepthStateReversed.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

		result = m_Device5->CreateDepthStencilState(&depthStencilDesc, m_DepthStencilStateWrite.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

		result = m_Device5->CreateDepthStencilState(&depthStencilDesc, m_DepthStencilStateWriteIgnoreDepth.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		depthStencilDesc.BackFace = depthStencilDesc.FrontFace;

		result = m_Device5->CreateDepthStencilState(&depthStencilDesc, m_DepthStencilStateRead.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

		result = m_Device5->CreateDepthStencilState(&depthStencilDesc, m_DepthStateReversedReadOnly.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		//Create Blend state
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = m_Device5->CreateBlendState(&blendDesc, m_BlendState.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = false;

		result = m_Device5->CreateBlendState(&blendDesc, m_AlphaToCoverageBlendState.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		result = m_Device5->CreateBlendState(&blendDesc, m_BlendStateAddition.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		//Create Rasterizer state
		D3D11_RASTERIZER_DESC rastDesc;
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));

		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = false;
		rastDesc.DepthBias = 0;
		rastDesc.SlopeScaledDepthBias = 0.0f;
		rastDesc.DepthBiasClamp = 0.0f;
		rastDesc.DepthClipEnable = true;
		rastDesc.ScissorEnable = false;
		rastDesc.MultisampleEnable = false;
		rastDesc.AntialiasedLineEnable = false;

		result = m_Device5->CreateRasterizerState(&rastDesc, m_RasterizerStateCullingOff.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		rastDesc.CullMode = D3D11_CULL_BACK;
		result = m_Device5->CreateRasterizerState(&rastDesc, m_RasterizerState.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		rastDesc.CullMode = D3D11_CULL_FRONT;
		result = m_Device5->CreateRasterizerState(&rastDesc, m_RasterizerStateFrontFaceCull.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		rastDesc.CullMode = D3D11_CULL_FRONT;
		rastDesc.DepthClipEnable = false;
		result = m_Device5->CreateRasterizerState(&rastDesc, m_RasterizerStateFrontFaceCullDepthClipOff.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		// Create Sampler states 
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		result = m_Device5->CreateSamplerState(&sampDesc, m_SamplerStatePoint.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		result = m_Device5->CreateSamplerState(&sampDesc, m_SamplerStateLinearMipPoint.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		result = m_Device5->CreateSamplerState(&sampDesc, m_SamplerStateLinear.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
		result = m_Device5->CreateSamplerState(&sampDesc, m_SamplerStateAnisotropic.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		result = m_Device5->CreateSamplerState(&sampDesc, m_SamplerCmp.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = 5;

		result = m_Device5->CreateSamplerState(&sampDesc, m_SamplerStateGrass.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));
	}


	void Globals::Update()
	{
		m_Devcon->OMSetDepthStencilState(m_DepthStateReversed.ptr(), 0);

		m_PerFrameBuffer.BindToVS(0);
		m_PerFrameBuffer.BindToPS(0);
		m_PerFrameBuffer.BindToGS(0);

		ID3D11SamplerState* pSS[] = { NULL, m_SamplerStateLinear.ptr(), m_SamplerStateGrass.ptr(), m_SamplerCmp.ptr() };

		if (m_CurrentSampler == 1)
			pSS[0] = m_SamplerStatePoint.ptr();
		else if (m_CurrentSampler == 2)
			pSS[0] = m_SamplerStateLinearMipPoint.ptr();
		else if (m_CurrentSampler == 3)
			pSS[0] = m_SamplerStateLinear.ptr();
		else if (m_CurrentSampler == 4)
			pSS[0] = m_SamplerStateAnisotropic.ptr();

		m_Devcon->PSSetSamplers(0, 4, pSS);

		this->UpdateConstants();
	}

	void Globals::UpdateConstants()
	{
		m_PerFrameBuffer.Update(&m_PerFrame, 1);
	}

	void Globals::SetCurrentSampler(int sampler)
	{
		m_CurrentSampler = sampler;
	}
}