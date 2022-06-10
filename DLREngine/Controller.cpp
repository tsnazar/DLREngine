#include "Controller.h"
#include "lights/Lighting.h"

namespace
{
	const int MOUSEWHEEL_THRESHOLD = 120;
	const float VELOCITY_ACCELERATION_FACTOR = 5.0f;
	const float VELOCITY_INC_DEC_FACTOR = 1.1f;
	const float ROTATION_SPEED = 0.5f;
	const float EXPOSURE_DELTA = 1.0f;
	const float FOV = 1.0472f; // 60 degrees
	const float ZNEAR = 100.0f;
	const float ZFAR = 0.1f;

	const math::Material green({ 0,1,0 }, { 0,0,0 }, { 0.04f, 0.04f, 0.04f }, 0.0f, 0.8f); // temp materials
	const math::Material blue({ 0,0,1 }, { 0,0,0 }, { 0.04f, 0.04f, 0.04f }, 0.0f, 0.8f); // temp materials
	const math::Material purple({ 0.4f , 0.0f , 0.8f }, { 0, 0, 0 }, { 0.04f, 0.04f, 0.04f }, 0.0f, 0.3f); // temp materials
}

Controller::Controller(Scene& scene, Camera& camera, MainWindow& window) :m_Scene(scene), m_Camera(camera), m_Window(window), m_Keys{ 0 }{}

void Controller::InitScene()
{
	m_Scene.AddPlaneToScene(DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT3(0, 1, 0), green);
	m_Scene.AddCubeToScene(math::Transform({ 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { -1.0f, 1.0f, 5.0f }), blue);
	m_Scene.AddCubeToScene(math::Transform({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { -3.0f, 2.0f, 3.0f }), purple);

	DirectX::XMFLOAT3 f0, albedo{ 1.00f, 0.77f, 0.28f };
	
	for (size_t x = 0; x < 10; ++x)
		for (size_t z = 0; z < 10; ++z)
		{
			float metal = x * 0.1f + 0.005f;
			float roughness = z * 0.1f + 0.005f;
			DirectX::XMStoreFloat3(&f0, math::mix(DirectX::XMVectorReplicate(0.04f), DirectX::XMLoadFloat3(&albedo), DirectX::XMVectorReplicate(metal)));
			m_Scene.AddSphereToScene(DirectX::XMFLOAT3(x, 1, z), 0.5f, math::Material(albedo, { 0,0,0 }, f0, metal, roughness));
		}

	m_Scene.AddPointLightToScene(DirectX::XMFLOAT3(5.0f, 7.0f, 5.0f), DirectX::XMFLOAT3(1500.0f, 1500.0f, 1500.0f), 0.5f);
	//m_Scene.AddSpotLightToScene(DirectX::XMFLOAT3(5.0f, 7.0f, 5.0f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT3(1500.0f, 1500.0f, 1500.0f), 0.5f, 0.6f, 0.8f);
	//m_Scene.AddDirLightToScene({ 0, -1, 0 }, { 1000.0f, 1000.0f, 1000.0f }, 0.01f);
	
	m_Camera.SetWorldOffset({ 4.0f, 10.0f, -1.0f });
	m_Camera.SetWorldAngles(FOV, 0.0f, 0.0f);
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
	float EV100 = m_Scene.GetEV100();

	if (m_Keys[size_t(Keys::KEY_R)])
	{
		m_Scene.ReflectionsOnOff();
		m_Keys[size_t(Keys::KEY_R)] = false;
	}

	if (m_Keys[size_t(Keys::KEY_G)])
	{
		m_Scene.GlobalIlluminationOnOff();
		m_Keys[size_t(Keys::KEY_G)] = false;
	}

	if (m_Keys[size_t(Keys::KEY_A)])
		offset.x = -m_CameraVelocity * delta;
	if (m_Keys[size_t(Keys::KEY_D)])
		offset.x = m_CameraVelocity * delta;
	if (m_Keys[size_t(Keys::KEY_E)])
		offset.y = m_CameraVelocity * delta;
	if (m_Keys[size_t(Keys::KEY_Q)])
		offset.y = -m_CameraVelocity * delta;
	if (m_Keys[size_t(Keys::KEY_S)])
		offset.z = -m_CameraVelocity * delta;
	if (m_Keys[size_t(Keys::KEY_W)])
		offset.z = m_CameraVelocity * delta;

	if (m_Keys[size_t(Keys::KEY_SHIFT)])
	{
		offset.x *= VELOCITY_ACCELERATION_FACTOR;
		offset.y *= VELOCITY_ACCELERATION_FACTOR;
		offset.z *= VELOCITY_ACCELERATION_FACTOR;
	}

	if (m_Keys[size_t(Keys::KEY_OEM_PLUS)])
		EV100 -= EXPOSURE_DELTA * delta;
	if(m_Keys[size_t(Keys::KEY_OEM_MINUS)])
		EV100 += EXPOSURE_DELTA * delta;

	if (m_MouseWheelDelta >= MOUSEWHEEL_THRESHOLD)
	{
		m_CameraVelocity *= VELOCITY_INC_DEC_FACTOR;
		m_MouseWheelDelta = 0;
	}
	else if (m_MouseWheelDelta <= -MOUSEWHEEL_THRESHOLD)
	{
		m_CameraVelocity /= VELOCITY_INC_DEC_FACTOR;
		m_MouseWheelDelta = 0;
	}

	if (m_Keys[size_t(Keys::KEY_LMB)])
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

	if (m_Keys[size_t(Keys::KEY_RMB)])
	{
		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(m_Window.GetWindow(), &pos);

		float xNDC = (2.0f * (pos.x + 0.5f) / (m_Window.GetClientWidth())) - 1.0f;
		float yNDC = 1.0f - (2.0f * (pos.y + 0.5f) / (m_Window.GetClientHeight()));

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

	m_Scene.SetEV100(EV100);
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
	case WM_KEYDOWN:
	{
		m_Keys[wParam] = true;
		m_Scene.SetGlobalIllumination(false);
		return 0;
	}

	case WM_KEYUP:
	{
		m_Keys[wParam] = false;
		return 0;
	}
	case WM_LBUTTONDOWN:
		m_Keys[size_t(Keys::KEY_LMB)] = true;
		m_Scene.SetGlobalIllumination(false);
		return 0;
	case WM_RBUTTONDOWN:
		m_Keys[size_t(Keys::KEY_RMB)] = true;
		m_Scene.SetGlobalIllumination(false);
		return 0;
	case WM_LBUTTONUP:
		m_Keys[size_t(Keys::KEY_LMB)] = false;
		m_Scene.SetGlobalIllumination(false);
		return 0;
	case WM_RBUTTONUP:
		m_Keys[size_t(Keys::KEY_RMB)] = false;
		m_Scene.SetGlobalIllumination(false);
		return 0;
	case WM_MOUSEWHEEL:
		m_MouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
		m_Scene.SetGlobalIllumination(false);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void Controller::Draw()
{
	m_Scene.Render(m_Window, m_Camera);
	m_Window.Flush();
}

