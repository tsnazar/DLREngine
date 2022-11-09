#include "DisintegrationInstances.h"
#include "windows/winapi.hpp"
#include <algorithm>
#include "Globals.h"
#include "ModelManager.h"
#include "ParticleSystem.h"

namespace engine
{
	DisintegrationInstances::DisintegrationInstances()
	{
		m_PerMeshConstants.Create<DirectX::XMFLOAT4X4>(D3D11_USAGE_DYNAMIC, nullptr, 1);
		m_PerMaterialConstants.Create<Material::Contants>(D3D11_USAGE_DYNAMIC, nullptr, 1);
	}

	void DisintegrationInstances::UpdateInstanceBuffers()
	{
		uint32_t totalInstances = 0;
		for (auto& perModel : m_PerModel)
			for (auto& perMesh : perModel.perMesh)
				for (const auto& perMaterial : perMesh.perMaterial)
					totalInstances += uint32_t(perMaterial.instances.size());

		if (totalInstances == 0)
			return;

		if (m_ResizeInstanceBuffer)
		{
			m_ResizeInstanceBuffer = false;
			m_InstanceBuffer.Create<GpuInstance>(D3D11_USAGE_DYNAMIC, nullptr, totalInstances);
		}

		GpuInstance* dst = static_cast<GpuInstance*>(m_InstanceBuffer.Map());

		uint32_t copiedNum = 0;

		auto& transforms = TransformSystem::Get().GetTransforms();

		for (const auto& perModel : m_PerModel)
		{
			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				for (const auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					for (const auto& instance : perMaterial.instances)
					{
						GpuInstance gpuInstance;
						LoadMatrixInArray(transforms[instance.transformID].GetTranspose(), gpuInstance.modelToWorld);
						gpuInstance.spherePos = instance.spherePos;
						gpuInstance.spawnTime = instance.spawnTime;
						gpuInstance.maxRadius = perModel.model->GetBoundingBox().radius();
						gpuInstance.objectID = instance.objectID;
						dst[copiedNum++] = gpuInstance;
					}
				}
			}
		}

		m_InstanceBuffer.UnMap();
	}

	void DisintegrationInstances::RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_ShadowsShader != nullptr && m_NoiseTexture != nullptr);

		m_ShadowsShader->SetShaders();

		Globals::Get().SetReversedDepthState();
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetRasterizerStateCullOff();

		ParticleSystem::Get().GetGpuConstants().BindToVS(ShaderDescription::Bindings::GPU_CONSTANTS);
		
		m_NoiseTexture->BindToPS(ShaderDescription::Bindings::NOISE_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		LightSystem::ShadowMapGeometryShaderConstants con;

		uint32_t renderedInstances = 0;
		for (const auto& perModel : m_PerModel)
		{
			perModel.model->Bind(ShaderDescription::Bindings::MESH_BUFFER);
			auto& subMeshes = perModel.model->GetSubMeshes();

			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				const Model::SubMesh& subMesh = subMeshes[meshIndex];
				const Mesh& mesh = perModel.model->GetMeshes()[meshIndex];

				m_PerMeshConstants.Update(&mesh.meshToModel, 1);
				m_PerMeshConstants.BindToVS(ShaderDescription::Bindings::MESH_TO_MODEL_BUFFER);

				for (const auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					if (perMaterial.instances.empty()) continue;

					uint32_t numInstances = static_cast<uint32_t>(perMaterial.instances.size());

					for (uint32_t i = 0; i < numLights; ++i)
					{
						memcpy(&con.matrices, &matrices[i], sizeof(DirectX::XMFLOAT4X4[6]));
						con.sliceOffset = i * 6;
						shadowMatrixBuffer.Update(&con, 1);
						shadowMatrixBuffer.BindToGS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);

						if (perModel.model->VertexBufferOnly())
							s_Devcon->DrawInstanced(subMesh.vertexNum, numInstances, subMesh.vertexOffset, renderedInstances);
						else
							s_Devcon->DrawIndexedInstanced(subMesh.indexNum, numInstances, subMesh.indexOffset, subMesh.vertexOffset, renderedInstances);
					}

					renderedInstances += numInstances;
				}
			}
		}
	}

	void DisintegrationInstances::SpawnParticles()
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_SpawnParticlesShader != nullptr && m_NoiseTexture != nullptr);

		m_SpawnParticlesShader->SetShaders();

		Globals::Get().SetDepthStateDisabled();
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetDefaultRasterizerState();

		ParticleSystem::Get().GetGpuConstants().BindToVS(ShaderDescription::Bindings::GPU_CONSTANTS);
		ParticleSystem::Get().GetGpuConstants().BindToGS(ShaderDescription::Bindings::GPU_CONSTANTS);

		m_NoiseTexture->BindToPS(ShaderDescription::Bindings::NOISE_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		uint32_t renderedInstances = 0;
		for (const auto& perModel : m_PerModel)
		{
			perModel.model->Bind(ShaderDescription::Bindings::MESH_BUFFER);
			auto& subMeshes = perModel.model->GetSubMeshes();

			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				const Model::SubMesh& subMesh = subMeshes[meshIndex];
				const Mesh& mesh = perModel.model->GetMeshes()[meshIndex];

				m_PerMeshConstants.Update(&mesh.meshToModel, 1);
				m_PerMeshConstants.BindToVS(ShaderDescription::Bindings::MESH_TO_MODEL_BUFFER);

				for (const auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					if (perMaterial.instances.empty()) continue;

					uint32_t numInstances = static_cast<uint32_t>(perMaterial.instances.size());

					if (perModel.model->VertexBufferOnly())
						s_Devcon->DrawInstanced(subMesh.vertexNum, numInstances, subMesh.vertexOffset, renderedInstances);
					else
						s_Devcon->DrawIndexedInstanced(subMesh.indexNum, numInstances, subMesh.indexOffset, subMesh.vertexOffset, renderedInstances);

					renderedInstances += numInstances;
				}
			}
		}
	}

	void DisintegrationInstances::Render(Sky::IblResources iblResources)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(iblResources.hasResources);

		ALWAYS_ASSERT(m_ForwardShader != nullptr && m_NoiseTexture != nullptr);

		m_ForwardShader->SetShaders();

		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);
		iblResources.reflection->BindToPS(ShaderDescription::Bindings::REFLECTION_TEXTURE);
		iblResources.reflectance->BindToPS(ShaderDescription::Bindings::REFLECTANCE_TEXTURE);

		m_NoiseTexture->BindToPS(ShaderDescription::Bindings::NOISE_TEXTURE);

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMapDimensions().BindToPS(ShaderDescription::Bindings::SHADOWMAP_DIMENSIONS);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		uint32_t renderedInstances = 0;
		for (const auto& perModel : m_PerModel)
		{
			perModel.model->Bind(ShaderDescription::Bindings::MESH_BUFFER);
			auto& subMeshes = perModel.model->GetSubMeshes();

			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				const Model::SubMesh& subMesh = subMeshes[meshIndex];
				const Mesh& mesh = perModel.model->GetMeshes()[meshIndex];

				m_PerMeshConstants.Update(&mesh.meshToModel, 1);
				m_PerMeshConstants.BindToVS(ShaderDescription::Bindings::MESH_TO_MODEL_BUFFER);

				for (const auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					if (perMaterial.instances.empty()) continue;

					const auto& material = perMaterial.material;

					m_PerMaterialConstants.Update(&material.constants, 1);
					m_PerMaterialConstants.BindToVS(ShaderDescription::Bindings::MATERIAL_CONSTANTS);
					m_PerMaterialConstants.BindToPS(ShaderDescription::Bindings::MATERIAL_CONSTANTS);

					material.texture->BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);

					if (material.roughness != nullptr)
						material.roughness->BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);

					if (material.metallic != nullptr)
						material.metallic->BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);

					if (material.normalMap != nullptr)
						material.normalMap->BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);

					uint32_t numInstances = static_cast<uint32_t>(perMaterial.instances.size());

					if (perModel.model->VertexBufferOnly())
						s_Devcon->DrawInstanced(subMesh.vertexNum, numInstances, subMesh.vertexOffset, renderedInstances);
					else
						s_Devcon->DrawIndexedInstanced(subMesh.indexNum, numInstances, subMesh.indexOffset, subMesh.vertexOffset, renderedInstances);

					renderedInstances += numInstances;
				}
			}
		}

	}

	void DisintegrationInstances::RenderToGBuffer()
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_GBufferShader != nullptr && m_NoiseTexture != nullptr);

		m_GBufferShader->SetShaders();

		ParticleSystem::Get().GetGpuConstants().BindToVS(ShaderDescription::Bindings::GPU_CONSTANTS);

		Globals::Get().SetDepthStencilStateWrite(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetRasterizerStateCullOff();

		m_NoiseTexture->BindToPS(ShaderDescription::Bindings::NOISE_TEXTURE);

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		uint32_t renderedInstances = 0;
		for (const auto& perModel : m_PerModel)
		{
			perModel.model->Bind(ShaderDescription::Bindings::MESH_BUFFER);
			auto& subMeshes = perModel.model->GetSubMeshes();

			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				const Model::SubMesh& subMesh = subMeshes[meshIndex];
				const Mesh& mesh = perModel.model->GetMeshes()[meshIndex];

				m_PerMeshConstants.Update(&mesh.meshToModel, 1);
				m_PerMeshConstants.BindToVS(ShaderDescription::Bindings::MESH_TO_MODEL_BUFFER);

				for (const auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					if (perMaterial.instances.empty()) continue;

					const auto& material = perMaterial.material;

					m_PerMaterialConstants.Update(&material.constants, 1);
					m_PerMaterialConstants.BindToVS(ShaderDescription::Bindings::MATERIAL_CONSTANTS);
					m_PerMaterialConstants.BindToPS(ShaderDescription::Bindings::MATERIAL_CONSTANTS);

					material.texture->BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);

					if (material.roughness != nullptr)
						material.roughness->BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);

					if (material.metallic != nullptr)
						material.metallic->BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);

					if (material.normalMap != nullptr)
						material.normalMap->BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);

					uint32_t numInstances = static_cast<uint32_t>(perMaterial.instances.size());

					if (perModel.model->VertexBufferOnly())
						s_Devcon->DrawInstanced(subMesh.vertexNum, numInstances, subMesh.vertexOffset, renderedInstances);
					else
						s_Devcon->DrawIndexedInstanced(subMesh.indexNum, numInstances, subMesh.indexOffset, subMesh.vertexOffset, renderedInstances);

					renderedInstances += numInstances;
				}
			}
		}
	}

	void DisintegrationInstances::AddInstance(Model* model, const std::vector<Material>& materials, uint32_t transformId, uint32_t objectID, DirectX::XMFLOAT3 spherePos)
	{
		m_ResizeInstanceBuffer = true;

		uint32_t numMeshes = model->GetSubMeshes().size();
		ALWAYS_ASSERT(numMeshes == materials.size());

		if (m_ModelIndexMap.find(model) == m_ModelIndexMap.end())
		{
			uint32_t modelIndex = m_PerModel.size();
			m_ModelIndexMap[model] = modelIndex;

			m_PerModel.emplace_back();
			m_PerModel[modelIndex].model = model;
			m_PerModel[modelIndex].perMesh.resize(numMeshes);
		}

		auto& perModel = m_PerModel[m_ModelIndexMap[model]];

		auto& globalValues = Globals::Get().GetPerFrameObj();

		DirectX::XMVECTOR sphereCenter = DirectX::XMVectorSet(spherePos.x, spherePos.y, spherePos.z, 1.0f);
		sphereCenter = DirectX::XMVector4Transform(sphereCenter, TransformSystem::Get().GetTransforms()[transformId].GetInvMatrix());
		DirectX::XMStoreFloat3(&spherePos, sphereCenter);

		Instance instance = { transformId, objectID, spherePos, globalValues.time };

		for (uint32_t i = 0; i < numMeshes; ++i)
		{
			auto& materialIndexMap = perModel.perMesh[i].materialIndexMap;

			if (materialIndexMap.find(materials[i]) == materialIndexMap.end())
			{
				uint32_t materialIndex = perModel.perMesh[i].perMaterial.size();
				materialIndexMap[materials[i]] = materialIndex;

				perModel.perMesh[i].perMaterial.emplace_back();
				perModel.perMesh[i].perMaterial[materialIndex].material = materials[i];
			}

			perModel.perMesh[i].perMaterial[materialIndexMap[materials[i]]].instances.push_back(instance);
		}
	}

	void DisintegrationInstances::DeleteInstance(Model* model, const std::vector<Material>& materials, uint32_t transformId)
	{
		m_ResizeInstanceBuffer = true;

		uint32_t numMeshes = model->GetSubMeshes().size();
		ALWAYS_ASSERT(numMeshes == materials.size());

		auto modelIter = m_ModelIndexMap.find(model);
		ALWAYS_ASSERT(modelIter != m_ModelIndexMap.end());

		auto& perModel = m_PerModel[modelIter->second];

		uint32_t materialsSize = 0;

		for (uint32_t i = 0; i < numMeshes; ++i)
		{
			auto& materialIndexMap = perModel.perMesh[i].materialIndexMap;

			auto materialIter = materialIndexMap.find(materials[i]);
			ALWAYS_ASSERT(materialIter != materialIndexMap.end());

			auto& instances = perModel.perMesh[i].perMaterial[materialIter->second].instances;

			auto instance = std::find_if(instances.begin(), instances.end(), [&](Instance a) {return a.transformID == transformId; });

			ALWAYS_ASSERT(instance != instances.end());

			instances.erase(instance);

			materialsSize += perModel.perMesh[i].perMaterial.size();
		}
	}
}