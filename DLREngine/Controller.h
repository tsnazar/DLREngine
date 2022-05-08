#pragma once
#include "Scene.h"
#include "Camera.h"
#include "windows/winapi.hpp"
#include <DirectXMath.h>

class Controller
{
public:
	Controller(Scene& scene, Camera& camera);

	void InitScene();

	void InitCamera(float fov, float aspect, float near, float far);

	void ProcessInput(float delta);

private:
	Scene& m_Scene;
	Camera& m_Camera;

	bool m_FirstMove = true;
	int m_LastX = 0, m_LastY = 0;
};
