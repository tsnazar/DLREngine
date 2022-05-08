#include "Controller.h"

#define VK_A 0x41
#define VK_D 0x44
#define VK_S 0x53
#define VK_W 0x57
#define VK_PRESSED 0x8000

const float VELOCITY = 1.0f;

Controller::Controller(Scene& scene, Camera& camera):m_Scene(scene), m_Camera(camera){}

void Controller::InitScene()
{
	std::vector<math::sphere>& spheres = m_Scene.GetSpheres();
	std::vector<math::plane>& planes = m_Scene.GetPlanes();

	spheres.emplace_back(DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f), 0.5f);
	planes.emplace_back(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f));
}

void Controller::InitCamera(float fov, float aspect, float near, float far)
{
	m_Camera.SetPerspective(fov, aspect, near, far);
}

void Controller::ProcessInput(float delta)
{
	DirectX::XMFLOAT3 offset = {0,0,0};
	
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

	m_Camera.AddRelativeOffset(offset);

	if (GetKeyState(VK_LBUTTON) & VK_PRESSED)
	{
		POINT pos;
		GetCursorPos(&pos);

		if (m_FirstMove)
		{
			m_FirstMove = false;
			m_LastX = pos.x;
			m_LastY = pos.y;
		}
		else
		{
			m_FirstMove = true;
		}

		float offsetX = (m_LastX - pos.x) * DirectX::XM_PI / 180.0f;
		float offsetY = (m_LastY - pos.y)* DirectX::XM_PI / 180.0f;

		m_LastX = pos.x;
		m_LastY = pos.y;

		DirectX::XMFLOAT3 ang = { offsetX, offsetY, 0 };
		m_Camera.AddRelativeAngles(ang);

	}
	else
	{
		m_FirstMove = true;
	}
}