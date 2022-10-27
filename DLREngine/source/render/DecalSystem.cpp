#include "DecalSystem.h"
#include "TransformSystem.h"
#include "ModelManager.h"
#include "Globals.h"
#include "TextureManager.h"

using namespace DirectX;

namespace engine
{
	const float DecalSystem::MAX_DEPTH = 1.0f;
	const float DecalSystem::MIN_DEPTH = 0.1f;

	DecalSystem* DecalSystem::s_Instance = nullptr;

	void DecalSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);
		s_Instance = new DecalSystem;
	}

	void DecalSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);
		delete s_Instance;
		s_Instance = nullptr;
	}

	void DecalSystem::Update()
	{
		std::vector<GpuInstance> gpuInstances;

		auto& transforms = TransformSystem::Get().GetTransforms();

		GpuInstance gpuInstance;
		for (auto& instance : m_Instances)
		{
			auto modelToWorld = TransformSystem::Get().GetTransforms()[instance.transformID].GetMatrix();

			LoadMatrixInArray(DirectX::XMMatrixTranspose(instance.decalToModel * modelToWorld), gpuInstance.decalToWorld);
			LoadMatrixInArray(DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, instance.decalToModel * modelToWorld)), gpuInstance.worldToDecal);
			
			gpuInstance.color = instance.color;
			gpuInstance.meshID = instance.meshID;
			gpuInstance.decalRight = instance.decalRight;

			gpuInstances.push_back(gpuInstance);
		}

		if(gpuInstances.size() > 0)
			m_InstanceBuffer.Create<GpuInstance>(D3D11_USAGE_DYNAMIC, gpuInstances.data(), gpuInstances.size());
	}

	void DecalSystem::SpawnDecal(Camera& camera, DirectX::XMFLOAT3 position, float halfSize, float NdotR, uint32_t transformID, uint32_t meshID)
	{
		float angle = ((rand() % 180) * DirectX::XM_PI) / 180.f;

		auto worldToModel = TransformSystem::Get().GetTransforms()[transformID].GetInvMatrix();

		DirectX::XMVECTOR decalPosition = DirectX::XMVectorSet(position.x, position.y, position.z, 1.0f);

		DirectX::XMMATRIX decalToModel, modelToDecal;
		decalToModel.r[0] = DirectX::XMVector4Transform(camera.Right(), worldToModel);
		decalToModel.r[1] = DirectX::XMVector4Transform(camera.Up(), worldToModel);
		decalToModel.r[2] = DirectX::XMVector4Transform(camera.Forward(), worldToModel);
		
		decalToModel.r[0] *= halfSize;
		decalToModel.r[1] *= halfSize;
		decalToModel.r[2] *= Lerp(MAX_DEPTH, MIN_DEPTH, NdotR);

		DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(decalToModel.r[2], angle);
		decalToModel = decalToModel * rotation;

		decalToModel.r[3] = DirectX::XMVector4Transform(decalPosition, worldToModel);

		Instance instance;

		instance.decalToModel = decalToModel;
		instance.transformID = transformID;
		instance.meshID = meshID;
		DirectX::XMStoreFloat3(&instance.decalRight, camera.Right());

		instance.color = DirectX::XMFLOAT3(rand() / static_cast<float>(RAND_MAX), rand() / static_cast<float>(RAND_MAX), rand() / static_cast<float>(RAND_MAX));

		m_Instances.push_back(instance);
	}
	
	void DecalSystem::RenderToGBuffer(Texture2D& depth, ConstantBuffer& dimensions, Texture2D& normals, Texture2D& meshIDs)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_GBufferShader != nullptr && m_DecalTexture != nullptr);

		m_GBufferShader->SetShaders();

		auto& cube = ModelManager::Get().GetUnitCube();

		Globals::Get().SetDepthStencilStateRead(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetRasterizerFrontFaceCull();

		depth.BindToPS(ShaderDescription::Bindings::DEPTH_GB_TEXTURE);
		normals.BindToPS(ShaderDescription::Bindings::NORMALS_GB_TEXTURE);
		m_DecalTexture->BindToPS(ShaderDescription::Bindings::DECAL_GB_TEXTURE);
		meshIDs.BindToPS(ShaderDescription::Bindings::MESHIDS_GB_TEXTURE);

		dimensions.BindToPS(ShaderDescription::Bindings::TARGET_DIMENSIONS_CONSTANTS);

		cube.Bind(ShaderDescription::Bindings::MESH_BUFFER);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		s_Devcon->DrawInstanced(cube.GetSubMeshes()[0].vertexNum, m_InstanceBuffer.GetVertexCount(), 0, cube.GetSubMeshes()[0].vertexOffset);
	}
}