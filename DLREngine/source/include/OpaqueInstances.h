#pragma once
#include "Model.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "Instance.h"

namespace engine
{
	class OpaqueInstances
	{
	public:
		struct Material
		{

		};

		struct PerMaterial
		{
			Texture2D* material;
			std::vector<InstanceTransform> instances;
		};

		struct PerMesh
		{
			std::unordered_map<Texture2D*, uint32_t> textureIndexMap;
			std::vector<PerMaterial> perMaterial;
		};

		struct PerModel
		{
			Model* model;
			std::vector<PerMesh> perMesh;
		};

	public:
		OpaqueInstances();

		void UpdateInstanceBuffers();
		
		void Render();

		void AddInstance(Model* model, std::vector<Texture2D*>& materials, InstanceTransform instance);

	private:
		bool m_ResizeInstanceBuffer = false;
		std::vector<PerModel> m_PerModel;
		std::unordered_map<Model*, uint32_t> m_ModelIndexMap;
		VertexBuffer m_InstanceBuffer;
		ConstantBuffer m_ConstantBuffer;
	};

}
