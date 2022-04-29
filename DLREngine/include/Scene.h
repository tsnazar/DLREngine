#pragma once
#include <Windows.h>
#include <vector>
#include "HitableObjs.h"
#include "windows/MainWindow.h"

class Scene
{
public:
	Scene();

	bool Render(const MainWindow& win);

	bool ProcessInput();
private:
	math::vec3 Color(const math::ray& r);
private:
	BITMAPINFO m_BMI;
	math::vec3 m_Offsets;
	HitableObjs m_Objects;
	std::vector<int32_t> m_Pixels;
};