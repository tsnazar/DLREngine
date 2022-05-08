#pragma once
#include <Windows.h>
#include <vector>

#include "windows/MainWindow.h"
#include "windows/winapi.hpp"
#include "sphere.h"
#include "plane.h"
#include "Camera.h"

#include <DirectXMath.h>

class Scene
{
public:
	Scene();

	bool Render(MainWindow& win, Camera& camera);

	std::vector<math::sphere>& GetSpheres() { return m_Spheres; }
	std::vector<math::plane>& GetPlanes() { return m_Planes; }
	
	DirectX::XMFLOAT3 GetOffset() const { return m_Offset; }

	void SetOffset(const DirectX::XMFLOAT3& offset) { m_Offset = offset; }

private:
	DirectX::XMFLOAT3 ComputeColor(const math::ray& castedRay);

private:
	DirectX::XMFLOAT3 m_Offset;
	std::vector<math::sphere> m_Spheres;
	std::vector<math::plane> m_Planes;
};