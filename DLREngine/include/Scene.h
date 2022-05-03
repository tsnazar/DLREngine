#pragma once
#include <Windows.h>
#include <vector>
#include "windows/MainWindow.h"
#include "sphere.h"

class Scene
{
public:
	Scene();

	bool Render(MainWindow& win);

	std::vector<math::sphere>& GetObjects() { return m_Objects; }
	
	math::vec3 GetOffset() const { return m_Offsets; }

	void SetOffset(const math::vec3& offset) { m_Offsets = offset; }

private:
	math::vec3 ComputeColor(const math::ray& castedRay);

private:
	math::vec3 m_Offsets;
	std::vector<math::sphere> m_Objects;
};