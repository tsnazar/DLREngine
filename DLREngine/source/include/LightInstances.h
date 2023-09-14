#pragma once
#include "Model.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "TransformSystem.h"
#include <xhash>

namespace engine
{
	class MeshSystem;

	class LightInstances
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t {  MESH_BUFFER = 0, INSTANCE_BUFFER = 1 };
		};

		struct Instance
		{
			DirectX::XMFLOAT3 color;
			uint32_t transformId;
		};

		struct GpuInstance
		{
			DirectX::XMFLOAT4 matrix[4];
			DirectX::XMFLOAT3 color;
			float padding;
		};

		struct PerModel
		{
			Model* model;
			std::vector<Instance> instanceRefs;
		};

	public:
		LightInstances();

		void SetShaders(Shader* forwardShader) { m_ForwardShader = forwardShader; }

		void UpdateInstanceBuffers();

		void Render();

		void AddInstance(Model* model, const DirectX::XMFLOAT3& color, const uint32_t& transformId);

	private:
		bool m_ResizeInstanceBuffer = false;
		std::vector<PerModel> m_PerModel;
		std::unordered_map<Model*, uint32_t> m_ModelIndexMap;
		VertexBuffer m_InstanceBuffer;

		Shader* m_ForwardShader;

		friend MeshSystem;
	};
}