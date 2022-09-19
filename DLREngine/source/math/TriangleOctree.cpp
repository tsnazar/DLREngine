#include "TriangleOctree.h"
#include "Model.h"
#include "Ray.h"
#include "MeshIntersection.h"
#include <algorithm>

namespace engine
{
	const int TriangleOctree::PREFFERED_TRIANGLE_COUNT = 32;
	const float TriangleOctree::MAX_STRETCHING_RATIO = 1.05f;

	inline const DirectX::XMFLOAT3 getPos(const Mesh& mesh, uint32_t triangleIndex, uint32_t vertexIndex)
	{
		uint32_t index = mesh.triangles.size() == 0 ? triangleIndex * 3 + vertexIndex : mesh.triangles[triangleIndex].indices[vertexIndex];
		return mesh.vertices[index].pos;
	}

	void TriangleOctree::initialize(const Mesh& mesh)
	{
		m_triangles.clear();
		m_triangles.shrink_to_fit();

		m_mesh = &mesh;
		m_children = nullptr;

		const DirectX::XMFLOAT3 eps = { 1e-5f, 1e-5f, 1e-5f };
		m_box = m_initialBox = { mesh.box.min - eps, mesh.box.max + eps };

		uint32_t size = mesh.triangles.size() == 0 ? mesh.vertices.size() / 3 : mesh.triangles.size();

		for (uint32_t i = 0; i < size; ++i)
		{
			const DirectX::XMFLOAT3 V1 = getPos(mesh, i, 0);
			const DirectX::XMFLOAT3 V2 = getPos(mesh, i, 1);
			const DirectX::XMFLOAT3 V3 = getPos(mesh, i, 2);

			DirectX::XMFLOAT3 P = (V1 + V2 + V3) / 3.f;

			bool inserted = addTriangle(i, V1, V2, V3, P);
			ALWAYS_ASSERT(inserted);
		}
	}

	void TriangleOctree::initialize(const Mesh& mesh, const Box& parentBox, const DirectX::XMFLOAT3& parentCenter, int octetIndex)
	{
		m_mesh = &mesh;
		m_children = nullptr;

		const float eps = 1e-5f;

		if (octetIndex % 2 == 0)
		{
			m_initialBox.min.x = parentBox.min.x;
			m_initialBox.max.x = parentCenter.x;
		}
		else
		{
			m_initialBox.min.x = parentCenter.x;
			m_initialBox.max.x = parentBox.max.x;
		}

		if (octetIndex % 4 < 2)
		{
			m_initialBox.min.y = parentBox.min.y;
			m_initialBox.max.y = parentCenter.y;
		}
		else 
		{
			m_initialBox.min.y = parentCenter.y;
			m_initialBox.max.y = parentBox.max.y;
		}

		if (octetIndex < 4)
		{
			m_initialBox.min.z = parentBox.min.z;
			m_initialBox.max.z = parentCenter.z;
		}
		else
		{
			m_initialBox.min.z = parentCenter.z;
			m_initialBox.max.z = parentBox.max.z;
		}

		m_box = m_initialBox;
		DirectX::XMFLOAT3 elongation = m_box.size() * (MAX_STRETCHING_RATIO - 1.0f);

		if (octetIndex % 2 == 0) m_box.max.x += elongation.x;
		else m_box.min.x -= elongation.x;

		if (octetIndex % 4 < 2) m_box.max.y += elongation.y;
		else m_box.min.y -= elongation.y;
	
		if (octetIndex < 4) m_box.max.z += elongation.z;
		else m_box.min.z -= elongation.z;
	}

	bool TriangleOctree::addTriangle(uint32_t triangleIndex, const DirectX::XMFLOAT3& V1, const DirectX::XMFLOAT3& V2, const DirectX::XMFLOAT3& V3, const DirectX::XMFLOAT3& center)
	{
		if (!m_initialBox.contains(center) || !m_box.contains(V1) || !m_box.contains(V2) || !m_box.contains(V3))
		{
			return false;
		}

		if (m_children == nullptr)
		{
			if (m_triangles.size() < PREFFERED_TRIANGLE_COUNT)
			{
				m_triangles.emplace_back(triangleIndex);
				return true;
			}
			else
			{
				DirectX::XMFLOAT3 C = (m_initialBox.min + m_initialBox.max) / 2.f;
				m_children.reset(new std::array<TriangleOctree, 8>());
				for (uint32_t i = 0; i < 8; ++i)
				{
					(*m_children)[i].initialize(*m_mesh, m_initialBox, C, i);
				}

				std::vector<uint32_t> newTriangles;
				for (uint32_t index : m_triangles)
				{
					const DirectX::XMFLOAT3& P1 = getPos(*m_mesh, index, 0);
					const DirectX::XMFLOAT3& P2 = getPos(*m_mesh, index, 1);
					const DirectX::XMFLOAT3& P3 = getPos(*m_mesh, index, 2);
					const DirectX::XMFLOAT3& P = (P1 + P2 + P3) / 3.f;

					int i = 0;
					for (; i < 8; ++i)
					{
						if ((*m_children)[i].addTriangle(index, P1, P2, P3, P))
							break;
					}

					if (i == 8)
						newTriangles.emplace_back(index);
				}

				m_triangles = std::move(newTriangles);
			}
		}

		int i = 0;
		for (; i < 8; ++i)
		{
			if ((*m_children)[i].addTriangle(triangleIndex, V1, V2, V3, center))
				break;
		}

		if (i == 8)
			m_triangles.emplace_back(triangleIndex);

		return true;
	}

	bool TriangleOctree::intersect(const Ray& ray, MeshIntersection& nearest) const
	{
		float boxT = nearest.t;
		if (!ray.intersect(boxT, m_box, true))
			return false;

		return intersectInternal(ray, nearest);
	}

	bool TriangleOctree::intersectInternal(const Ray& ray, MeshIntersection& nearest) const
	{
		{
			float boxT = nearest.t;
			if (!ray.intersect(boxT, m_box, true))
				return false;
		}

		bool found = false;

		for (uint32_t i = 0; i < m_triangles.size(); ++i)
		{
			const DirectX::XMFLOAT3& V1 = getPos(*m_mesh, m_triangles[i], 0);
			const DirectX::XMFLOAT3& V2 = getPos(*m_mesh, m_triangles[i], 2);
			const DirectX::XMFLOAT3& V3 = getPos(*m_mesh, m_triangles[i], 1);

			if (ray.intersect(nearest, V1, V2, V3))
			{
				nearest.triangle = i;
				found = true;
			}
		}

		if (!m_children) return found;
		
		struct OctantIntersection
		{
			int index;
			float t;
		};

		std::array<OctantIntersection, 8> boxIntersections;

		for (int i = 0; i < 8; ++i)
		{
			if ((*m_children)[i].m_box.contains(ray.origin))
			{
				boxIntersections[i].index = i;
				boxIntersections[i].t = 0.f;
			}
			else {
				float boxT = nearest.t;
				if (ray.intersect(boxT, (*m_children)[i].m_box, true))
				{
					boxIntersections[i].index = i;
					boxIntersections[i].t = boxT;
				}
				else {
					boxIntersections[i].index = -1;
				}
			}
		}
		
		std::sort(boxIntersections.begin(), boxIntersections.end(), [](const OctantIntersection& A, const OctantIntersection& B) -> bool
				{ return A.t < B.t;});

		for (int i = 0; i < 8; ++i)
		{
			if (boxIntersections[i].index < 0 || boxIntersections[i].t > nearest.t)
				continue;

			if ((*m_children)[boxIntersections[i].index].intersectInternal(ray, nearest))
			{
				found = true;
			}
		}
			
		return found;
	}
}