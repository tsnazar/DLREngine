#pragma once
#include "Scene.h"
#include "Camera.h"
#include "windows/MainWindow.h"
#include "windows/winapi.hpp"
#include <DirectXMath.h>

class Controller
{
public:
	Controller(Scene& scene, Camera& camera, MainWindow& window);

	void InitScene();

	void InitCamera();

	void MoveCamera(const DirectX::XMFLOAT3& offset, const DirectX::XMFLOAT3& angles);

	void ProcessInput(float delta);

	LRESULT ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Draw();
private:
	Scene& m_Scene;
	Camera& m_Camera;
	MainWindow& m_Window;

	bool m_FirstMoveLB = true;
	int m_LastXLB = 0, m_LastYLB = 0;

	bool m_FirstMoveRB = true;
	math::Intersection m_Inter;

	std::unique_ptr<IObjectMover> m_Mover = nullptr;
};
