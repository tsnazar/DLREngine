#include "LightSystem.h"
#include "Globals.h"
#include "MeshSystem.h"
#include "ModelManager.h"

namespace
{
	
}

namespace engine
{
	LightSystem* LightSystem::s_Instance = nullptr;

	const float LightSystem::SHADOW_MAP_ASPECT = (float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT;
	const float LightSystem::SHADOW_MAP_NEAR = 25.0f;
	const float LightSystem::SHADOW_MAP_FAR = 0.1f;
	const DirectX::XMMATRIX LightSystem::SHADOW_MAP_PROJECTION = DirectX::XMMatrixPerspectiveFovLH(1.57f, SHADOW_MAP_ASPECT, SHADOW_MAP_NEAR, SHADOW_MAP_FAR);

	LightSystem::LightSystem()
	{
		m_ShadowMatricesBuffer.Create<ShadowMapConstants>(D3D11_USAGE_DYNAMIC, nullptr, 1);
	}

	void LightSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);
		s_Instance = new LightSystem;
	}

	void LightSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);
		delete s_Instance;
		s_Instance = nullptr;
	}

	void LightSystem::AddPointLight(const PointLight& light)
	{
		TransformSystem::Transform transform;
		transform.position = light.position;
		transform.rotation = { 0,0,0 };
		transform.scale = { light.radius , light.radius , light.radius };

		auto& transforms = TransformSystem::Get().GetTransforms();
		uint32_t id = transforms.insert(transform);

		PointLightRef ref;
		ref.transformId = id;
		ref.radiance = light.radiance;
		ref.radius = light.radius;
		GenerateShadowTransforms(ref.matrices, light.position);

		ALWAYS_ASSERT(m_NumLights < MAX_POINT_LIGHTS);
		m_PointLightRefs[m_NumLights] = ref;

		MeshSystem::Get().GetLightInstances().AddInstance(&ModelManager::Get().GetModel("Sphere"), light.radiance, id);

		++m_NumLights;
	}

	void LightSystem::InitShadowMaps()
	{
		ALWAYS_ASSERT(m_NumLights > 0);

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		desc.Width = SHADOW_MAP_WIDTH;
		desc.Height = SHADOW_MAP_HEIGHT;
		desc.MipLevels = 1;
		desc.ArraySize = m_NumLights * 6;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		//shaderResourceViewDesc.Texture2D.MipLevels = 1;
		//shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.TextureCubeArray.First2DArrayFace = 0;
		shaderResourceViewDesc.TextureCubeArray.NumCubes = m_NumLights;
		shaderResourceViewDesc.TextureCubeArray.MipLevels = 1;
		shaderResourceViewDesc.TextureCubeArray.MostDetailedMip = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		depthStencilViewDesc.Texture2DArray.ArraySize = 6;
		depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;

		DepthTarget target;
		target.CreateFromDescription(desc, &shaderResourceViewDesc, &depthStencilViewDesc);

		for (uint32_t i = 0; i < m_NumLights; ++i)
		{
			m_ShadowMaps[i] = target;
			depthStencilViewDesc.Texture2DArray.FirstArraySlice = i * 6;
			m_ShadowMaps[i].RecreateDSV(depthStencilViewDesc);
		}
	}

	void LightSystem::Update()
	{
		auto& perFrame = Globals::Get().GetPerFrameObj();
		auto& transforms = TransformSystem::Get().GetTransforms();
		
		for (uint32_t i = 0; i < m_NumLights; ++i)
		{
			PointLight light; 
			light.position = transforms[m_PointLightRefs[i].transformId].position;
			light.radiance = m_PointLightRefs[i].radiance;
			light.radius = m_PointLightRefs[i].radius;

			perFrame.pointLights[i] = light;

			GenerateShadowTransforms(m_PointLightRefs[i].matrices, light.position);
		}
	}

	void LightSystem::RenderToShadowMaps()
	{
		D3D11_VIEWPORT viewport;

		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = SHADOW_MAP_WIDTH;
		viewport.Height = SHADOW_MAP_HEIGHT;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		ID3D11RenderTargetView* const pRTV[1] = { NULL };

		s_Devcon->RSSetViewports(1, &viewport);

		ShaderManager::Get().GetShader("shadows").SetShaders();

		auto& transforms = TransformSystem::Get().GetTransforms();

		ID3D11ShaderResourceView* const pSRV[1] = { NULL };
		s_Devcon->PSSetShaderResources(7, 1, pSRV);

		for (uint32_t i = 0; i < m_NumLights; ++i)
		{
			ShadowMapConstants con;
			memcpy(&con.matrices, &m_PointLightRefs[i].matrices, 6 * sizeof(DirectX::XMFLOAT4X4));
			con.lightPos = transforms[m_PointLightRefs[i].transformId].position;
			con.farPlane = SHADOW_MAP_NEAR;

			m_ShadowMatricesBuffer.Update(&con, 1);
			m_ShadowMatricesBuffer.BindToGS(1);
			m_ShadowMatricesBuffer.BindToPS(1);

			s_Devcon->OMSetRenderTargets(1, pRTV, m_ShadowMaps[i].GetDepthView().ptr());
			s_Devcon->ClearDepthStencilView(m_ShadowMaps[i].GetDepthView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			MeshSystem::Get().RenderToShadowMap();
		}
	}

	void LightSystem::GenerateShadowTransforms(DirectX::XMFLOAT4X4* arr, const DirectX::XMFLOAT3& position)
	{
		using namespace DirectX;

		DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
		DirectX::XMStoreFloat4x4(&arr[0], DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, pos + DirectX::XMVectorSet(1.0, 0.0, 0.0, 0.0), DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0)) * SHADOW_MAP_PROJECTION));
		DirectX::XMStoreFloat4x4(&arr[1], DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, pos + DirectX::XMVectorSet(-1.0, 0.0, 0.0, 0.0), DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0)) * SHADOW_MAP_PROJECTION));
		DirectX::XMStoreFloat4x4(&arr[2], DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, pos + DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0), DirectX::XMVectorSet(0.0, 0.0, -1.0, 0.0)) * SHADOW_MAP_PROJECTION));
		DirectX::XMStoreFloat4x4(&arr[3], DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, pos + DirectX::XMVectorSet(0.0, -1.0, 0.0, 0.0), DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0)) * SHADOW_MAP_PROJECTION));
		DirectX::XMStoreFloat4x4(&arr[4], DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, pos + DirectX::XMVectorSet(0.0, 0.0, 1.0, 0.0), DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0)) * SHADOW_MAP_PROJECTION));
		DirectX::XMStoreFloat4x4(&arr[5], DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, pos + DirectX::XMVectorSet(0.0, 0.0, -1.0, 0.0), DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0)) * SHADOW_MAP_PROJECTION));
	}

}