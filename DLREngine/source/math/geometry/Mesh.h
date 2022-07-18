#pragma once
#include <vector>
#include "Triangle.h"

namespace math
{
	struct Mesh
	{
		std::vector<Triangle> triangles;
		
		bool Intersect(const Ray& ray, Intersection& record) const;

		static Mesh createCube()
		{
			//CW - front face, CCW - back face
			Mesh m;
			////front
			m.triangles.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f));
			m.triangles.emplace_back(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f));
			//////back
			m.triangles.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f));
			m.triangles.emplace_back(DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f));
			//////left
			m.triangles.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f));
			m.triangles.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f));
			//////right
			m.triangles.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f));
			m.triangles.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f));
			//////top
			m.triangles.emplace_back(DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			m.triangles.emplace_back(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f));
			//////bottom
			m.triangles.emplace_back(DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f));
			m.triangles.emplace_back(DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f));
			
			return m;
		}
	};
}