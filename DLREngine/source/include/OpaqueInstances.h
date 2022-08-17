#pragma once
#include "Model.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include <xhash>

namespace engine
{
	class OpaqueInstances
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t { ALBEDO_TEXTURE = 0, MESH_BUFFER = 0, MESH_TO_MODEL_BUFFER = 1, INSTANCE_BUFFER = 1 };
		};

		struct Instance
		{
			DirectX::XMFLOAT4 matrix[4];
		};

		struct Material
		{
			Texture2D* texture;

			bool operator<(const Material& other)
			{
				return texture < texture;
			}

			friend bool operator==(const Material& lhs, const Material& rhs)
			{
				return lhs.texture == rhs.texture;
			}

			struct hash {
				size_t operator()(const Material& v) const {
					return std::hash<Texture2D*>{}(v.texture);
				}
			};
		};

		struct PerMaterial
		{
			Material material;
			std::vector<Instance> instances;
		};

		struct PerMesh
		{
			std::unordered_map<Material, uint32_t, Material::hash> materialIndexMap;
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

		void AddInstance(Model* model, std::vector<Material>& materials, Instance instance);

	private:
		bool m_ResizeInstanceBuffer = false;
		std::vector<PerModel> m_PerModel;
		std::unordered_map<Model*, uint32_t> m_ModelIndexMap;
		VertexBuffer m_InstanceBuffer;
		ConstantBuffer m_ConstantBuffer;
	};
}

