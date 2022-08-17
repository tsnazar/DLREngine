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

		struct InstanceRef
		{
			DirectX::XMFLOAT3 color;
			uint32_t transformId;
		};

		struct Instance
		{
			DirectX::XMFLOAT3 color;
			DirectX::XMFLOAT4 matrix[4];
		};

		struct PerModel
		{
			Model* model;
			std::vector<InstanceRef> instanceRefs;
		};

	public:
		LightInstances();

		void UpdateInstanceBuffers();

		void Render();

		void AddInstance(Model* model, const DirectX::XMFLOAT3& color, const uint32_t& transformId);

	private:
		bool m_ResizeInstanceBuffer = false;
		std::vector<PerModel> m_PerModel;
		std::unordered_map<Model*, uint32_t> m_ModelIndexMap;
		VertexBuffer m_InstanceBuffer;

		friend MeshSystem;
	};
}