#include "Controller.h"

#define VK_A 0x41
#define VK_D 0x44
#define VK_S 0x53
#define VK_W 0x57
#define VK_PRESSED 0x8000

const float VELOCITY = 100.0f;

Controller::Controller(Scene& scene):m_Scene(scene){}

void Controller::InitScene()
{
	std::vector<math::sphere>& objs = m_Scene.GetObjects();

	objs.emplace_back(DirectX::XMFLOAT3(50.0f, 50.0f, -51.0f), 50);
}

void Controller::ProcessInput(float delta)
{
	DirectX::XMFLOAT3 offset = m_Scene.GetOffset();

	if (GetKeyState(VK_A) & VK_PRESSED)
		offset.x -= VELOCITY * delta;
	if (GetKeyState(VK_D) & VK_PRESSED)
		offset.x += VELOCITY * delta;
	if (GetKeyState(VK_S) & VK_PRESSED)
		offset.y -= VELOCITY * delta;
	if (GetKeyState(VK_W) & VK_PRESSED)
		offset.y += VELOCITY * delta;

	if (GetKeyState(VK_RBUTTON) & VK_PRESSED)
	{
		POINT pos;
		GetCursorPos(&pos);

		if (m_FirstMove)
		{
			m_FirstMove = false;
			m_LastX = pos.x;
			m_LastY = pos.y;
		}

		DirectX::XMFLOAT3 path(pos.x - m_LastX, m_LastY - pos.y, 0);
		offset.x +=  path.x;
		offset.y +=  path.y;
		offset.z +=  path.z;

		m_LastX = pos.x;
		m_LastY = pos.y;
	}
	else
	{
		m_FirstMove = true;
	}

	m_Scene.SetOffset(offset);
}