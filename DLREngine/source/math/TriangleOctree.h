#pragma once
#include "Box.h"
#include <cmath>
#include <vector>
#include <memory>
#include <array>
#include "windows/winapi.hpp"

namespace engine
{
	struct Ray;

	struct Mesh;

	struct MeshIntersection;

	class TriangleOctree
	{
	public:
		const static int PREFFERED_TRIANGLE_COUNT;
		const static float MAX_STRETCHING_RATIO;

		void clear() { m_mesh = nullptr; }

		bool inited() const { return m_mesh != nullptr; }

		void initialize(const Mesh& mesh);

		bool intersect(const Ray& ray, MeshIntersection& nearest) const;

	protected:
		const Mesh* m_mesh = nullptr;
		std::vector<uint32_t> m_triangles;
		Box m_box;
		Box m_initialBox;

		std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

		void initialize(const Mesh& mesh, const Box& parentBox, const DirectX::XMFLOAT3& parentCenter, int octetIndex);

		bool addTriangle(uint32_t triangleIndex, const DirectX::XMFLOAT3& V1, const DirectX::XMFLOAT3& V2, const DirectX::XMFLOAT3& V3, const DirectX::XMFLOAT3& center);

		bool intersectInternal(const Ray& ray, MeshIntersection& nearest) const;
	};
}