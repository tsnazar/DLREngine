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

	bool ProcessInput(float delta);
private:
	math::vec3 ComputeColor(const math::ray& castedRay);
private:
	math::vec3 m_Offsets;
	std::vector<math::sphere> m_Objects;
};