#include "Controller.h"

namespace
{
	const SHORT VK_A = 0x41;
	const SHORT VK_D = 0x44;
	const SHORT VK_E = 0x45;
	const SHORT VK_Q = 0x51;
	const SHORT VK_S = 0x53;
	const SHORT VK_W = 0x57;
	const SHORT VK_PRESSED = 0x8000;

	const float VELOCITY = 1.0f;
	const float ROTATION_SPEED = 0.5f;
	const float FOV = 0.5f;
	const float ZNEAR = 100.0f;
	const float ZFAR = 0.1f;

	const math::Material red({ 1,0,0 }, { 0,0,0 }, 64.0f, 0.5f); // temp materials
	const math::Material green({ 0,1,0 }, { 0,0,0 }, 16.0f, 0.7f); // temp materials
	const math::Material blue({ 0,0,1 }, { 0,0,0 }, 32.0f, 0.7f); // temp materials
	const math::Material azure({ 0.0f , 1.0f , 0.5f }, { 0,0,0 }, 32.0f, 0.3f); // temp materials
	const math::Material purple({ 0.4f , 0.0f , 0.8f }, { 0,0,0 }, 128.0f, 0.5f); // temp materials
}

Controller::Controller(Scene& scene, Camera& camera, MainWindow& window):m_Scene(scene), m_Camera(camera), m_Window(window){}

void Controller::InitScene()
{
	m_Scene.AddSphereToScene(DirectX::XMFLOAT3(0, 0, 2.0f), 0.5f, red);
	m_Scene.AddSphereToScene(DirectX::XMFLOAT3(0, 0, -2.0f), 0.5f, azure);
	m_Scene.AddCubeToScene(math::Transform({ 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 5.0f }), blue);
	m_Scene.AddCubeToScene(math::Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { -3.0f, 2.0f, 3.0f }), purple);
	m_Scene.AddPlaneToScene(DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT3(0, 1, 0), green);

	m_Scene.AddDirLightToScene(DirectX::XMFLOAT3(0.0f, -1.0f, 5.0f), DirectX::XMFLOAT3(0.3f, 0.5f, 0.5f));
	m_Scene.AddSpotLightToScene(DirectX::XMFLOAT3(0.0f, 4.0f, 5.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 2.0f, 0.5f, 0.6f);
	m_Scene.AddPointLightToScene(DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f), DirectX::XMFLOAT3(0.5f, 1.0f, 0.5f), 2.0f);
	m_Scene.AddPointLightToScene(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 2.0f);
}

void Controller::InitCamera()
{
	int width = m_Window.GetImageWidth();
	int height = m_Window.GetImageHeight();

	m_Camera.SetPerspective(FOV, (float)width / (float)height, ZNEAR, ZFAR);
}

void Controller::MoveCamera(const DirectX::XMFLOAT3 & offset, const DirectX::XMFLOAT3 & angles)
{
	m_Camera.AddRelativeOffset(offset);
	m_Camera.AddRelativeAngles(angles);
	m_Camera.UpdateMatrices();
}

void Controller::ProcessInput(float delta)
{
	DirectX::XMFLOAT3 offset = { 0, 0, 0 };
	DirectX::XMFLOAT3 ang = { 0, 0, 0 };

	if (GetKeyState(VK_A) & VK_PRESSED)
		offset.x = -VELOCITY * delta;
	if (GetKeyState(VK_D) & VK_PRESSED)
		offset.x = VELOCITY * delta;
	if (GetKeyState(VK_SPACE) & VK_PRESSED)
		offset.y = VELOCITY * delta;
	if (GetKeyState(VK_CONTROL) & VK_PRESSED)
		offset.y = -VELOCITY * delta;
	if (GetKeyState(VK_S) & VK_PRESSED)
		offset.z = -VELOCITY * delta;
	if (GetKeyState(VK_W) & VK_PRESSED)
		offset.z = VELOCITY * delta;

	if (GetKeyState(VK_Q) & VK_PRESSED)
		ang.z = ROTATION_SPEED * delta;
	if (GetKeyState(VK_E) & VK_PRESSED)
		ang.z = -ROTATION_SPEED * delta;

	if (GetKeyState(VK_LBUTTON) & VK_PRESSED)
	{
		POINT pos;
		GetCursorPos(&pos);

		if (m_FirstMoveLB)
		{
			m_FirstMoveLB = false;
			m_LastXLB = pos.x;
			m_LastYLB = pos.y;
		}

		ang.x = (pos.x - m_LastXLB) * DirectX::XM_PI / 180.0f * ROTATION_SPEED * delta;
		ang.y = (pos.y - m_LastYLB) * DirectX::XM_PI / 180.0f * ROTATION_SPEED * delta;
	}
	else
	{
		m_FirstMoveLB = true;
	}

	if (GetKeyState(VK_RBUTTON) & VK_PRESSED)
	{
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(m_Window.GetWindow(), &pos);

		float xNDC = (2.0f * pos.x / (m_Window.GetClientWidth())) - 1.0f;
		float yNDC = 1.0f - (2.0f * pos.y / (m_Window.GetClientHeight()));

		DirectX::XMVECTOR cameraPos = m_Camera.Position();
		DirectX::XMVECTOR worldPos = m_Camera.Unproject(DirectX::XMVectorSet(xNDC, yNDC, 1.0f, 1.0f));
		DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(worldPos, cameraPos));

		DirectX::XMFLOAT3 dir, org;
		DirectX::XMStoreFloat3(&dir, direction);
		DirectX::XMStoreFloat3(&org, cameraPos);

		math::Ray ray(org, dir);

		if (m_FirstMoveRB)
		{
			m_FirstMoveRB = false;

			Scene::IntersectionQuery queue;
			queue.intersection.Reset();
			queue.mover = &m_Mover;

			m_Scene.FindIntersection(ray, queue);

			m_Inter = queue.intersection;
		}

		if (m_Mover != nullptr)
		{
			DirectX::XMFLOAT3 off;
			DirectX::XMStoreFloat3(&off, DirectX::XMVectorSubtract(ray.PointAtLine(m_Inter.t), DirectX::XMLoadFloat3(&m_Inter.pos)));
			m_Mover->Move(off);
		}

		DirectX::XMStoreFloat3(&m_Inter.pos, ray.PointAtLine(m_Inter.t));
	}
	else
	{
		m_FirstMoveRB = true;
		m_Mover = nullptr;
	}

	MoveCamera(offset, ang);
}

LRESULT Controller::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	{
		int width = m_Window.GetImageWidth();
		int height = m_Window.GetImageHeight();

		m_Camera.SetPerspective(FOV, (float)width / (float)height, ZNEAR, ZFAR);
		return 0;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Controller::Draw()
{
	m_Scene.Render(m_Window, m_Camera);
	m_Window.Flush();
}

