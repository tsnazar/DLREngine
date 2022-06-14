#pragma once
#include "Scene.h"
#include "Camera.h"
#include "windows/MainWindow.h"
#include "windows/winapi.hpp"
#include <DirectXMath.h>

namespace
{
	const short NUM = 256;
}

class Controller
{
public:
	enum class Keys { KEY_LMB = 0x01, KEY_RMB = 0x02, KEY_SHIFT = 0x10, KEY_CONTROL = 0x11, KEY_SPACE = 0x20, KEY_A = 0x41, KEY_D = 0x44, KEY_E = 0x45, KEY_G = 0x47, KEY_Q = 0x51, KEY_R = 0x52, KEY_S = 0x53, KEY_W = 0x57, KEY_OEM_PLUS = 0xBB, KEY_OEM_MINUS = 0xBD};
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
	bool m_Keys[NUM];

	int m_MouseWheelDelta = 0;
	float m_CameraVelocity = 5.0f;
	float m_RotationSpeed;

	bool m_FirstMoveLB = true;
	int m_LastXLB = 0, m_LastYLB = 0;

	bool m_FirstMoveRB = true;
	math::Intersection m_Inter;

	std::unique_ptr<IObjectMover> m_Mover = nullptr;
};
