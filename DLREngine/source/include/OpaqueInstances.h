#pragma once
#include "Model.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "HashUtils.h"
#include "TransformSystem.h"
#include <xhash>

namespace engine
{
	class MeshSystem; 

	class OpaqueInstances
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t { ALBEDO_TEXTURE = 0, ROUGHNESS_TEXTURE = 1, METALLIC_TEXTURE = 2, NORMAL_MAP_TEXTURE = 3, MESH_BUFFER = 0, MESH_TO_MODEL_BUFFER = 1, INSTANCE_BUFFER = 1, MATERIAL_CONSTANTS = 2};
		};

		struct Instance
		{
			DirectX::XMFLOAT4 matrix[4];
		};

		struct Material
		{
			struct Contants
			{
				enum
				{
					hasRoughness = 1 << 0,
					hasMetallic = 1 << 1,
					hasNormals = 1 << 2,
				};
				int flags;
				float roughness, metallic;
				float placeholder;
			};

			Material() = default;
			Material(Texture2D* texture, Texture2D* roughnessTex, Texture2D* metallicTex, Texture2D* normalMap, float roughness, float metallic)
				: texture(texture), roughness(roughnessTex), metallic(metallicTex), normalMap(normalMap)
			{
				constants.flags = 0;
				constants.flags |= roughnessTex == nullptr ? 0 : Material::Contants::hasRoughness;
				constants.flags |= metallicTex == nullptr ? 0 : Material::Contants::hasMetallic;
				constants.flags |= normalMap == nullptr ? 0 : Material::Contants::hasNormals;
				constants.roughness = roughness;
				constants.metallic = metallic;
			}

			Texture2D* texture;
			Texture2D* roughness;
			Texture2D* metallic;
			Texture2D* normalMap;
			Contants constants;

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
					size_t hash = 0;
					hashCombine(hash, v.texture);
					hashCombine(hash, v.roughness);
					hashCombine(hash, v.metallic);
					hashCombine(hash, v.normalMap);
					return hash;
				}
			};
		};

		struct PerMaterial
		{
			Material material;
			std::vector<uint32_t> instanceIDs;
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

		void AddInstance(Model* model, std::vector<Material>& materials, TransformSystem::Transform transform);

	private:
		bool m_ResizeInstanceBuffer = false;
		std::vector<PerModel> m_PerModel;
		std::unordered_map<Model*, uint32_t> m_ModelIndexMap;
		VertexBuffer m_InstanceBuffer;
		ConstantBuffer m_PerMeshConstants;
		ConstantBuffer m_PerMaterialConstants;

		friend MeshSystem;
	};
}

