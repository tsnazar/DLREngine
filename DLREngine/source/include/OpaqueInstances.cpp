#include "OpaqueInstances.h"
#include "windows/winapi.hpp"

namespace engine
{
	OpaqueInstances::OpaqueInstances()
	{
		m_PerMeshConstants.Create<DirectX::XMFLOAT4X4>(D3D11_USAGE_DYNAMIC, nullptr, 1);
		m_PerMaterialConstants.Create<Material::Contants>(D3D11_USAGE_DYNAMIC, nullptr, 1);
	}

	void OpaqueInstances::UpdateInstanceBuffers()
	{
		uint32_t totalInstances = 0;
		for (auto& perModel : m_PerModel)
			for (auto& perMesh : perModel.perMesh)
				for (const auto& perMaterial : perMesh.perMaterial)
					totalInstances += uint32_t(perMaterial.instanceIDs.size());

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
					for (const auto& id : perMaterial.instanceIDs)
					{
						GpuInstance instance;
						LoadMatrixInArray(transforms[id].GetTranspose(), instance.matrix);
						dst[copiedNum++] = instance;
					}
				}
			}
		}

		m_InstanceBuffer.UnMap();
	}

	void OpaqueInstances::Render()
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ShaderManager::Get().GetShader("instanceBRDF").SetShaders();
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
					if (perMaterial.instanceIDs.empty()) continue;

					const auto& material = perMaterial.material;

					m_PerMaterialConstants.Update(&material.constants, 1);
					m_PerMaterialConstants.BindToVS(ShaderDescription::Bindings::MATERIAL_CONSTANTS);
					m_PerMaterialConstants.BindToPS(ShaderDescription::Bindings::MATERIAL_CONSTANTS);

					material.texture->BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);

					if(material.roughness != nullptr)
						material.roughness->BindToPS(ShaderDescription::Bindings::ROUGHNESS_TEXTURE);
					
					if (material.metallic != nullptr)
						material.metallic->BindToPS(ShaderDescription::Bindings::METALLIC_TEXTURE);

					if(material.normalMap != nullptr)
						material.normalMap->BindToPS(ShaderDescription::Bindings::NORMAL_MAP_TEXTURE);

					uint32_t numInstances = static_cast<uint32_t>(perMaterial.instanceIDs.size());

					if (perModel.model->VertexBufferOnly())
						s_Devcon->DrawInstanced(subMesh.vertexNum, numInstances, subMesh.vertexOffset, renderedInstances);
					else
						s_Devcon->DrawIndexedInstanced(subMesh.indexNum, numInstances, subMesh.indexOffset, subMesh.vertexOffset, renderedInstances);
					
					renderedInstances += numInstances;
				}
			}
		}
	}

	void OpaqueInstances::AddInstance(Model* model, std::vector<Material>& materials, TransformSystem::Transform transform)
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
		auto& transforms = TransformSystem::Get().GetTransforms();
		uint32_t instanceID = transforms.insert(transform);

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

			perModel.perMesh[i].perMaterial[materialIndexMap[materials[i]]].instanceIDs.push_back(instanceID);
		}
	}
}

