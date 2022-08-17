#include "OpaqueInstances.h"
#include "windows/winapi.hpp"

namespace engine
{
	OpaqueInstances::OpaqueInstances()
	{
		m_ConstantBuffer.Create<DirectX::XMFLOAT4X4>(D3D11_USAGE_DYNAMIC, nullptr, 1);
	}

	void OpaqueInstances::UpdateInstanceBuffers()
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
			m_InstanceBuffer.Create<Instance>(D3D11_USAGE_DYNAMIC, nullptr, totalInstances);
		}

		Instance* dst = static_cast<Instance*>(m_InstanceBuffer.Map());

		uint32_t copiedNum = 0;

		for (const auto& perModel : m_PerModel)
		{
			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				for (const auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					auto& instances = perMaterial.instances;
					uint32_t numModelInstances = instances.size();

					for (uint32_t index = 0; index < numModelInstances; ++index)
					{
						dst[copiedNum++] = instances[index];
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

		ShaderManager::Get().GetShader("instance").SetShaders();
		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		uint32_t renderedInstances = 0;
		for (const auto& perModel : m_PerModel)
		{
			perModel.model->Bind(ShaderDescription::Bindings::MESH_BUFFER);
			auto& subMeshes = perModel.model->GetSubMeshes();

			for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
			{
				const Model::SubMesh& subMesh = subMeshes[meshIndex];

				m_ConstantBuffer.Update(&subMesh.meshToModel, 1);
				m_ConstantBuffer.BindToVS(ShaderDescription::Bindings::MESH_TO_MODEL_BUFFER);

				for (const auto& perMaterial : perModel.perMesh[meshIndex].perMaterial)
				{
					if (perMaterial.instances.empty()) continue;

					const auto& material = perMaterial.material;

					material.texture->BindToPS(ShaderDescription::Bindings::ALBEDO_TEXTURE);

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

	void OpaqueInstances::AddInstance(Model* model, std::vector<Material>& materials, Instance instance)
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
}

