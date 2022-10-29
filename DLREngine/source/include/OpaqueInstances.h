#pragma once
#include "Model.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "HashUtils.h"
#include "TransformSystem.h"
#include "Sky.h"
#include <xhash>
#include "LightSystem.h"

namespace engine
{
	class MeshSystem; 

	class OpaqueInstances
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t { ALBEDO_TEXTURE = 0, ROUGHNESS_TEXTURE = 1, METALLIC_TEXTURE = 2, NORMAL_MAP_TEXTURE = 3, 
									   SHADOWMAP_TEXTURE = 4, IRRADIANCE_TEXTURE = 5, REFLECTION_TEXTURE = 6, REFLECTANCE_TEXTURE = 7, 
									   MESH_BUFFER = 0, MESH_TO_MODEL_BUFFER = 1, INSTANCE_BUFFER = 1, MATERIAL_CONSTANTS = 2, TARGET_DIMENSIONS_CONSTANTS = 4,
									   SHADOWMAP_MATRICES = 1, SHADOWMAP_DIMENSIONS = 3,
										DEPTH_DS_TEXTURE = 0, ALBEDO_DS_TEXTURE = 1, NORMALS_DS_TEXTURE = 2, ROUGHMETALLIC_DS_TEXTURE = 3, EMISSION_DS_TEXTURE = 5, 
										EMISSION_GB_TEXTURE = 4,
										STENCIL_REF = 1,
			};
		};

		struct Instance
		{
			uint32_t transformID;
			uint32_t objectID;
		};

		struct GpuInstance
		{
			DirectX::XMFLOAT4 matrix[4];
			uint32_t objectID;
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
		OpaqueInstances();

		void SetShaders(Shader* forwardShader, Shader* deferredShader, Shader* deferredIBLShader, Shader* GBufferShader, Shader* shadowsShader)
		{
			m_ForwardShader = forwardShader;
			m_DeferredShader = deferredShader;
			m_DeferredIBLShader = deferredIBLShader;
			m_GBufferShader = GBufferShader;
			m_ShadowsShader = shadowsShader;
		}

		void UpdateInstanceBuffers();
		
		void Render(Sky::IblResources iblResources);

		void RenderToGBuffer();

		void ResolveGBuffer(Sky::IblResources iblResources, Texture2D& depth, Texture2D& albedo, Texture2D& normals,
			Texture2D& roughnessMetallic, Texture2D& emission, ConstantBuffer& dimensions);

		void RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights);

		void AddInstance(Model* model, std::vector<Material>& materials, uint32_t transformId, uint32_t& objectID);

	private:
		bool m_ResizeInstanceBuffer = false;
		std::vector<PerModel> m_PerModel;
		std::unordered_map<Model*, uint32_t> m_ModelIndexMap;
		VertexBuffer m_InstanceBuffer;
		ConstantBuffer m_PerMeshConstants;
		ConstantBuffer m_PerMaterialConstants;

		Shader* m_ForwardShader;
		Shader* m_DeferredShader;
		Shader* m_DeferredIBLShader;
		Shader* m_GBufferShader;
		Shader* m_ShadowsShader;

		friend MeshSystem;
	};
}

