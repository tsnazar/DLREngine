#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace engine
{
	class Model
	{
	public:
		struct SubMesh
		{
			DirectX::XMFLOAT4X4 meshToModel;
			uint32_t vertexOffset;
			uint32_t indexOffset;
			uint32_t vertexNum;
			uint32_t indexNum;
		};
	public:

		Model& LoadFromFile(const std::string& filepath);

		std::vector<SubMesh>& GetSubMeshes() { return m_Meshes; }

		void Bind();

		bool VertexBufferOnly() { return m_VertexBufferOnly; }
		void SetVertexBufferOnly(bool value) { m_VertexBufferOnly = value; }

		void InitUnitCube();

	private:
		std::vector<SubMesh> m_Meshes;
		VertexBuffer m_Vertices;
		IndexBuffer m_Indices;
		bool m_VertexBufferOnly = false;
	};
}