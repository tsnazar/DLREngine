#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Box.h"
#include "TriangleOctree.h"

namespace engine
{
	struct Mesh
	{
		using Vertex = VertexPosTexNorTanBitan;
		
		struct Triangle { uint32_t indices[3]; };

		void UpdateOctree()
		{
			tree.clear();
			tree.initialize(*this);
		}

		Box box;
		DirectX::XMFLOAT4X4 meshToModel;
		DirectX::XMFLOAT4X4 modelToMesh;
		TriangleOctree tree;
		std::vector<Vertex> vertices;
		std::vector<Triangle> triangles;
	};

	class Model
	{
	public:
		struct SubMesh
		{
			uint32_t vertexOffset;
			uint32_t indexOffset;
			uint32_t vertexNum;
			uint32_t indexNum;
		};
	public:

		Model& LoadFromFile(const std::string& filepath);

		std::vector<SubMesh>& GetSubMeshes() { return m_Submeshes; }

		std::vector<Mesh>& GetMeshes() { return m_Meshes; }

		void Bind(uint32_t slot);

		bool VertexBufferOnly() { return m_VertexBufferOnly; }

		void SetVertexBufferOnly(bool value) { m_VertexBufferOnly = value; }

		void InitUnitCube();

		void InitUnitSphere();

		bool Intersect(const Ray& ray, MeshIntersection& intersection, const DirectX::XMFLOAT4X4& transform);

	private:
		std::vector<SubMesh> m_Submeshes;
		std::vector<Mesh> m_Meshes;
		VertexBuffer m_Vertices;
		IndexBuffer m_Indices;
		bool m_VertexBufferOnly = false;
	};
}