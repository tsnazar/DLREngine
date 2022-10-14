#include "DissolutionInstances.h"
#include "windows/winapi.hpp"
#include <algorithm>
#include "Globals.h"

namespace engine
{
	DissolutionInstances::DissolutionInstances()
	{
		m_PerMeshConstants.Create<DirectX::XMFLOAT4X4>(D3D11_USAGE_DYNAMIC, nullptr, 1);
		m_PerMaterialConstants.Create<Material::Contants>(D3D11_USAGE_DYNAMIC, nullptr, 1);
	}

	void DissolutionInstances::UpdateInstanceBuffers()
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
						LoadMatrixInArray(transforms[instance.transformID].GetTranspose(), gpuInstance.matrix);
						gpuInstance.time = 1.0f - instance.lifeTime / instance.maxLifeTime;
						dst[copiedNum++] = gpuInstance;
					}
				}
			}
		}

		m_InstanceBuffer.UnMap();
	}

	void DissolutionInstances::Update(float dt)
	{
		for (auto& perModel : m_PerModel)
		{
			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				for (auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					for (auto& instance : perMaterial.instances)
					{
						instance.lifeTime -= dt;
					}

					std::sort(perMaterial.instances.begin(), perMaterial.instances.end(), [](Instance a, Instance b) {return a.lifeTime > b.lifeTime; });
				}
			}
		}
	}

	void DissolutionInstances::Render(Sky::IblResources iblResources)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(iblResources.hasResources);

		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);
		iblResources.reflection->BindToPS(ShaderDescription::Bindings::REFLECTION_TEXTURE);
		iblResources.reflectance->BindToPS(ShaderDescription::Bindings::REFLECTANCE_TEXTURE);

		if (m_DissolutionMode == 0)
		{
			Globals::Get().SetAlphaToCoverageBlendState();
			ShaderManager::Get().GetShader("dissolutionAlpha").SetShaders();
		}
		else if(m_DissolutionMode == 1)
		{
			Globals::Get().SetDefaultBlendState();
			ShaderManager::Get().GetShader("dissolution").SetShaders();
		}

		TextureManager::Get().GetTexture("noise").BindToPS(ShaderDescription::Bindings::NOISE_TEXTURE);

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

	void DissolutionInstances::AddInstance(Model* model, const std::vector<Material>& materials, uint32_t transformId, float animationTime)
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
		Instance instance = { transformId, animationTime, animationTime };

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
	
	void DissolutionInstances::DeleteInstance(Model* model, const std::vector<Material>& materials, uint32_t transformId)
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

			if (instances.size() == 0)
			{
				perModel.perMesh[i].perMaterial.erase(perModel.perMesh[i].perMaterial.begin() + materialIter->second);
				materialIndexMap.erase(materialIter);
			}

			materialsSize += perModel.perMesh[i].perMaterial.size();
		}

		if (materialsSize == 0)
		{
			m_PerModel.erase(m_PerModel.begin() + modelIter->second);
			m_ModelIndexMap.erase(modelIter);
		}

	}
}