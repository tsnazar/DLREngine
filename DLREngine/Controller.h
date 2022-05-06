#pragma once
#include "Scene.h"
#include <DirectXMath.h>

class Controller
{
public:
	Controller(Scene& scene);

	void InitScene();

	void ProcessInput(float delta);

private:
	Scene& m_Scene;

	bool m_FirstMove = true;
	int m_LastX = 0, m_LastY = 0;
};
