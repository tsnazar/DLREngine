#pragma once
#include <Windows.h>
#include <vector>
#include "windows/MainWindow.h"
#include "sphere.h"

#include <DirectXMath.h>

class Scene
{
public:
	Scene();

	bool Render(MainWindow& win);

	std::vector<math::sphere>& GetObjects() { return m_Objects; }
	
	DirectX::XMFLOAT3 GetOffset() const { return m_Offset; }

	void SetOffset(const DirectX::XMFLOAT3& offset) { m_Offset = offset; }

private:
	DirectX::XMFLOAT3 ComputeColor(const math::ray& castedRay);

private:
	DirectX::XMFLOAT3 m_Offset;
	std::vector<math::sphere> m_Objects;
};