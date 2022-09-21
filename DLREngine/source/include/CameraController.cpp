#include "CameraController.h"
#include "Application.h"
#include "windows/winapi.hpp"

namespace
{
	const int MOUSEWHEEL_THRESHOLD = 120;
	const float VELOCITY_ACCELERATION_FACTOR = 5.0f;
	const float VELOCITY_INC_DEC_FACTOR = 1.1f;
}

namespace engine
{
	CameraController::CameraController(float fov, float aspect, float near, float far)
		: m_Camera(fov, aspect, near, far), m_FOV(fov), m_ZNear(near), m_ZFar(far)
	{
		m_RotationSpeed = 360.0f / Application::Get().GetWindow().GetClientWidth();
	}

	void CameraController::OnUpdate(float delta)
	{
		const MainWindow& win = Application::Get().GetWindow();
		
		DirectX::XMFLOAT3 offset = { 0, 0, 0 };
		DirectX::XMFLOAT3 angles = { 0, 0, 0 };

		if (win.IsKeyPressed(Key::A))
			offset.x = -m_CameraVelocity * delta;
		if (win.IsKeyPressed(Key::D))
			offset.x = m_CameraVelocity * delta;
		if (win.IsKeyPressed(Key::E))
			offset.y = m_CameraVelocity * delta;
		if (win.IsKeyPressed(Key::Q))
			offset.y = -m_CameraVelocity * delta;
		if (win.IsKeyPressed(Key::S))
			offset.z = -m_CameraVelocity * delta;
		if (win.IsKeyPressed(Key::W))
			offset.z = m_CameraVelocity * delta;

		if (win.IsKeyPressed(Key::SHIFT))
		{
			offset.x *= VELOCITY_ACCELERATION_FACTOR;
			offset.y *= VELOCITY_ACCELERATION_FACTOR;
			offset.z *= VELOCITY_ACCELERATION_FACTOR;
		}

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

		if (win.IsKeyPressed(Key::LMB))
		{
			POINT pos;
			GetCursorPos(&pos);

			if (m_FirstMoveLB)
			{
				m_FirstMoveLB = false;
				m_LastXLB = pos.x;
				m_LastYLB = pos.y;
			}

			angles.x = (pos.x - m_LastXLB) * DirectX::XM_PI / 180.0f * m_RotationSpeed * delta;
			angles.y = (pos.y - m_LastYLB) * DirectX::XM_PI / 180.0f * m_RotationSpeed * delta;
		}
		else
		{
			m_FirstMoveLB = true;
		}

		m_Camera.AddRelativeOffset(offset);
		m_Camera.AddRelativeAngles(angles);
		m_Camera.UpdateMatrices();
	}

	void CameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNCTION(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(CameraController::OnWindowResized));
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_MouseWheelDelta += e.GetYOffset();
		return true;
	}
	
	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_RotationSpeed = 360.0f / e.GetWidth();
		m_Camera.SetPerspective(m_FOV, (float)e.GetWidth() / (float)e.GetHeight(), m_ZNear, m_ZFar);
		return true;
	}
	
	Ray CameraController::GetPickingRay()
	{
		auto& window = Application::Get().GetWindow();

		POINT pos;
		GetCursorPos(&pos);
		ScreenToClient(window.GetWindow(), &pos);

		float xNDC = (2.0f * (pos.x + 0.5f) / (window.GetClientWidth())) - 1.0f;
		float yNDC = 1.0f - (2.0f * (pos.y + 0.5f) / (window.GetClientHeight()));

		auto& camera = GetCamera();
		DirectX::XMVECTOR cameraPos = camera.Position();
		DirectX::XMVECTOR worldPos = camera.Unproject(DirectX::XMVectorSet(xNDC, yNDC, 1.0f, 1.0f));
		DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(worldPos, cameraPos);

		DirectX::XMFLOAT3 dir, org;
		DirectX::XMStoreFloat3(&dir, direction);
		DirectX::XMStoreFloat3(&org, cameraPos);

		return Ray(org, dir);
	}
}
