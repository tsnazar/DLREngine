#include "OpaqueInstances.h"
#include "windows/winapi.hpp"
#include "Globals.h"
#include "ModelManager.h"

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
						gpuInstance.meshID = instance.meshID;
						dst[copiedNum++] = gpuInstance;
					}
				}
			}
		}

		m_InstanceBuffer.UnMap();
	}

	void OpaqueInstances::RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_ShadowsShader != nullptr);

		m_ShadowsShader->SetShaders();

		//ShaderManager::Get().GetShader("opaqueShadows").SetShaders();

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

	void OpaqueInstances::ResolveGBuffer(Sky::IblResources iblResources, Texture2D& depth, Texture2D& albedo, Texture2D& normals,
		Texture2D& roughnessMetallic, Texture2D& emission, ConstantBuffer& dimensions)
	{
		auto& instanceBuffer = LightSystem::Get().GetDeferedShadingLightInstances();

		if (instanceBuffer.GetVertexCount() == 0 || !instanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(iblResources.hasResources);

		ALWAYS_ASSERT(m_DeferredShader != nullptr && m_DeferredIBLShader != nullptr);

		m_DeferredShader->SetShaders();
		//ShaderManager::Get().GetShader("opaqueDS").SetShaders();

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMatricesBuffer().BindToPS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);
		LightSystem::Get().GetShadowMapDimensions().BindToPS(ShaderDescription::Bindings::SHADOWMAP_DIMENSIONS);

		dimensions.BindToPS(ShaderDescription::Bindings::TARGET_DIMENSIONS_CONSTANTS);

		Globals::Get().SetBlendStateAddition();
		Globals::Get().SetDepthStencilStateRead(1);
		Globals::Get().SetRasterizerStateFrontFaceCullDepthClipOff();

		depth.BindToPS(ShaderDescription::Bindings::DEPTH_DS_TEXTURE);
		albedo.BindToPS(ShaderDescription::Bindings::ALBEDO_DS_TEXTURE);
		normals.BindToPS(ShaderDescription::Bindings::NORMALS_DS_TEXTURE);
		roughnessMetallic.BindToPS(ShaderDescription::Bindings::ROUGHMETALLIC_DS_TEXTURE);
		emission.BindToPS(ShaderDescription::Bindings::EMISSION_DS_TEXTURE);

		Model& sphere = ModelManager::Get().GetUnitSphere();
		sphere.Bind(0);

		instanceBuffer.SetBuffer(1);

		s_Devcon->DrawInstanced(sphere.GetSubMeshes()[0].vertexNum, instanceBuffer.GetVertexCount(), 0, sphere.GetSubMeshes()[0].vertexOffset);

		Globals::Get().SetBlendStateAddition();
		Globals::Get().SetDepthStencilStateRead(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetDefaultRasterizerState();

		emission.BindToPS(ShaderDescription::Bindings::EMISSION_GB_TEXTURE);
		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);
		iblResources.reflection->BindToPS(ShaderDescription::Bindings::REFLECTION_TEXTURE);
		iblResources.reflectance->BindToPS(ShaderDescription::Bindings::REFLECTANCE_TEXTURE);

		m_DeferredIBLShader->SetShaders();
		//ShaderManager::Get().GetShader("opaqueIBLDS").SetShaders();

		s_Devcon->Draw(3, 0);
	}


	void OpaqueInstances::Render(Sky::IblResources iblResources)
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(iblResources.hasResources);

		ALWAYS_ASSERT(m_ForwardShader != nullptr);
		m_ForwardShader->SetShaders();
		//ShaderManager::Get().GetShader("opaque").SetShaders();
	
		Globals::Get().SetReversedDepthState();
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetDefaultRasterizerState();

		iblResources.irradiance->BindToPS(ShaderDescription::Bindings::IRRADIANCE_TEXTURE);
		iblResources.reflection->BindToPS(ShaderDescription::Bindings::REFLECTION_TEXTURE);
		iblResources.reflectance->BindToPS(ShaderDescription::Bindings::REFLECTANCE_TEXTURE);

		LightSystem::Get().GetShadowMap().BindToPS(ShaderDescription::Bindings::SHADOWMAP_TEXTURE);
		LightSystem::Get().GetShadowMatricesBuffer().BindToPS(ShaderDescription::Bindings::SHADOWMAP_MATRICES);
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

	void OpaqueInstances::RenderToGBuffer()
	{
		if (m_InstanceBuffer.GetVertexCount() == 0 || !m_InstanceBuffer.IsValid())
			return;

		ALWAYS_ASSERT(m_GBufferShader != nullptr);

		m_GBufferShader->SetShaders();
		//ShaderManager::Get().GetShader("opaqueGB").SetShaders();

		Globals::Get().SetDepthStencilStateWrite(ShaderDescription::Bindings::STENCIL_REF);
		Globals::Get().SetDefaultBlendState();
		Globals::Get().SetDefaultRasterizerState();

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

	void OpaqueInstances::AddInstance(Model* model, std::vector<Material>& materials, uint32_t transformId, uint32_t& meshID)
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

			perModel.perMesh[i].perMaterial[materialIndexMap[materials[i]]].instances.push_back({ transformId, meshID++ });
		}
	}
}

