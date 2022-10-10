#pragma once
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Texture2D.h"
#include "Model.h"
#include "HashUtils.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "TransformSystem.h"
#include "Sky.h"
#include "LightSystem.h"

namespace engine
{
	class MeshSystem;

	class DissolutionInstances
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t {
				ALBEDO_TEXTURE = 0, ROUGHNESS_TEXTURE = 1, METALLIC_TEXTURE = 2, NORMAL_MAP_TEXTURE = 3,
				SHADOWMAP_TEXTURE = 4, IRRADIANCE_TEXTURE = 5, REFLECTION_TEXTURE = 6, REFLECTANCE_TEXTURE = 7, NOISE_TEXTURE = 8,
				MESH_BUFFER = 0, MESH_TO_MODEL_BUFFER = 1, INSTANCE_BUFFER = 1, MATERIAL_CONSTANTS = 2,
				SHADOWMAP_MATRICES = 1, SHADOWMAP_DIMENSIONS = 3
			};
		};

		struct GpuInstance
		{
			DirectX::XMFLOAT4 matrix[4];
			float time;
		};

		struct Instance
		{
			uint32_t transformID;
			float lifeTime;
			float maxLifeTime;
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
					flipNormals = 1 << 3,
				};
				int flags;
				float roughness, metallic;
				float padding;
			};

			Material() = default;
			Material(Texture2D* texture, Texture2D* roughnessTex, Texture2D* metallicTex, Texture2D* normalMap, float roughness, float metallic, bool flipNormals = false)
				: texture(texture), roughness(roughnessTex), metallic(metallicTex), normalMap(normalMap)
			{
				constants.flags = 0;
				constants.flags |= roughnessTex == nullptr ? 0 : Material::Contants::hasRoughness;
				constants.flags |= metallicTex == nullptr ? 0 : Material::Contants::hasMetallic;
				constants.flags |= normalMap == nullptr ? 0 : Material::Contants::hasNormals;
				constants.flags |= (normalMap == nullptr || !flipNormals) ? 0 : Material::Contants::flipNormals;
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
		DissolutionInstances();

		void UpdateInstanceBuffers();

		void Update(float dt);

		void Render(Sky::IblResources iblResources);

		//void RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights);

		void AddInstance(Model* model, const std::vector<Material>& materials, uint32_t transformId, float animationTime);

		void DeleteInstance(Model* model, const std::vector<Material>& materials, uint32_t transformId);

		std::vector<PerModel>& GetPerModelVector() { return m_PerModel; }

		void SetDissolutionMode(int mode) { m_DissolutionMode = mode; }

	private:
		bool m_ResizeInstanceBuffer = false;
		std::vector<PerModel> m_PerModel;
		std::unordered_map<Model*, uint32_t> m_ModelIndexMap;
		VertexBuffer m_InstanceBuffer;
		ConstantBuffer m_PerMeshConstants;
		ConstantBuffer m_PerMaterialConstants;

		int m_DissolutionMode = 0;

		friend MeshSystem;
	};
}