#include "LightInstances.h"

namespace engine
{
	LightInstances::LightInstances()
	{
	}

	void LightInstances::UpdateInstanceBuffers()
	{
		uint32_t totalInstances = 0;
		for (auto& perModel : m_PerModel)
			totalInstances += uint32_t(perModel.instanceRefs.size());

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
			for (const auto& ref : perModel.instanceRefs)
			{
				GpuInstance instance;
				instance.color = ref.color;
				LoadMatrixInArray(transforms[ref.transformId].GetTranspose(), instance.matrix);
				dst[copiedNum++] = instance;
			}
		}

		m_InstanceBuffer.UnMap();
	}

	void LightInstances::Render()
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ShaderManager::Get().GetShader("lightInstance").SetShaders();
		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		uint32_t renderedInstances = 0;
		for (const auto& perModel : m_PerModel)
		{
			perModel.model->Bind(ShaderDescription::Bindings::MESH_BUFFER);
			auto& subMeshes = perModel.model->GetSubMeshes();

			uint32_t numInstances = static_cast<uint32_t>(perModel.instanceRefs.size());

			for (uint32_t index = 0; index < subMeshes.size(); ++index)
			{
				const Model::SubMesh& subMesh = subMeshes[index];

				if (perModel.model->VertexBufferOnly())
					s_Devcon->DrawInstanced(subMesh.vertexNum, numInstances, subMesh.vertexOffset, renderedInstances);
				else
					s_Devcon->DrawIndexedInstanced(subMesh.indexNum, numInstances, subMesh.indexOffset, subMesh.vertexOffset, renderedInstances);

				renderedInstances += numInstances;
			}
		}
	}

	void LightInstances::RenderToShadowMap()
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		m_InstanceBuffer.SetBuffer(ShaderDescription::Bindings::INSTANCE_BUFFER);

		uint32_t renderedInstances = 0;
		for (const auto& perModel : m_PerModel)
		{
			perModel.model->Bind(ShaderDescription::Bindings::MESH_BUFFER);
			auto& subMeshes = perModel.model->GetSubMeshes();

			uint32_t numInstances = static_cast<uint32_t>(perModel.instanceRefs.size());

			for (uint32_t index = 0; index < subMeshes.size(); ++index)
			{
				const Model::SubMesh& subMesh = subMeshes[index];

				if (perModel.model->VertexBufferOnly())
					s_Devcon->DrawInstanced(subMesh.vertexNum, numInstances, subMesh.vertexOffset, renderedInstances);
				else
					s_Devcon->DrawIndexedInstanced(subMesh.indexNum, numInstances, subMesh.indexOffset, subMesh.vertexOffset, renderedInstances);

				renderedInstances += numInstances;
			}
		}
	}

	void LightInstances::AddInstance(Model* model, const DirectX::XMFLOAT3& color, const uint32_t& transformId)
	{
		m_ResizeInstanceBuffer = true;

		if (m_ModelIndexMap.find(model) == m_ModelIndexMap.end())
		{
			uint32_t modelIndex = m_PerModel.size();
			m_ModelIndexMap[model] = modelIndex;

			m_PerModel.emplace_back();
			m_PerModel[modelIndex].model = model;
		}

		auto& perModel = m_PerModel[m_ModelIndexMap[model]];
		auto& transforms = TransformSystem::Get().GetTransforms();

		Instance instance;
		instance.color = color;
		instance.transformId = transformId;

		perModel.instanceRefs.push_back(instance);
	}
}