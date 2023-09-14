#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Box.h"
#include "TriangleOctree.h"
#include "Ray.h"

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

		bool Intersect(const Ray& ray, MeshIntersection& intersection, const DirectX::XMMATRIX& worldToModelMatrix, const DirectX::XMMATRIX& modelToWorldMatrix)
		{
			bool intersect = false;

			Ray meshRay;

			DirectX::XMVECTOR origin = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&ray.origin), 1.0f);
			DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&ray.direction);
			origin = DirectX::XMVector4Transform(origin, worldToModelMatrix);
			direction = DirectX::XMVector4Transform(direction, worldToModelMatrix);

			DirectX::XMMATRIX modelToMeshMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&modelToMesh));
			DirectX::XMVECTOR meshOrigin = DirectX::XMVector4Transform(origin, modelToMeshMatrix);
			DirectX::XMVECTOR meshDirection = DirectX::XMVector4Transform(direction, modelToMeshMatrix);

			DirectX::XMStoreFloat3(&meshRay.origin, meshOrigin);
			DirectX::XMStoreFloat3(&meshRay.direction, meshDirection);

			intersect |= tree.intersect(meshRay, intersection);

			if (intersect)
			{
				DirectX::XMMATRIX meshToModelMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&meshToModel));
				DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&intersection.normal);
				normal = DirectX::XMVector4Transform(normal, meshToModelMatrix);
				normal = DirectX::XMVector4Transform(normal, modelToWorldMatrix);
				normal = DirectX::XMVector3Normalize(normal);
				DirectX::XMStoreFloat3(&intersection.normal, normal);
				DirectX::XMStoreFloat3(&intersection.pos, ray.PointAtLine(intersection.t));
			}

			return intersect;
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

		bool Intersect(const Ray& ray, MeshIntersection& intersection, const DirectX::XMMATRIX& worldToModel);

		Box& GetBoundingBox() { return m_BoundingBox; }

	private:
		std::vector<SubMesh> m_Submeshes;
		std::vector<Mesh> m_Meshes;
		Box m_BoundingBox;
		VertexBuffer m_Vertices;
		IndexBuffer m_Indices;
		bool m_VertexBufferOnly = false;
	};
}