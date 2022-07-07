#include "Controller.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "Shader.h"
#include "InputLayout.h"
#include "VertexBuffer.h"

namespace
{
	const int MOUSEWHEEL_THRESHOLD = 120;
	const float VELOCITY_ACCELERATION_FACTOR = 5.0f;
	const float VELOCITY_INC_DEC_FACTOR = 1.1f;
	const float EXPOSURE_DELTA = 1.0f;
	const float FOV = 1.0472f; // 60 degrees
	const float ZNEAR = 100.0f;
	const float ZFAR = 0.1f;
	const float FRAME_DURATION = 1.f / 60.f;

}

namespace engine
{
	Controller::Controller(Scene& scene, Camera& camera, MainWindow& window) :m_Scene(scene), m_Camera(camera), m_Window(window), m_Keys{ 0 }
	{
		m_RotationSpeed = 360.0f / window.GetClientWidth();
	}

	void Controller::InitScene()
	{
		m_Scene.GetShader().LoadFromFile(engine::VertexType::PosCol, "shaders/shader.shader");

		engine::VertexPosCol vertex_data_array[] = {
			{DirectX::XMFLOAT3{0.0f,  0.5f,  0.0f}, DirectX::XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}}, // point at top
			{DirectX::XMFLOAT3{0.5f, -0.5f,  0.0f }, DirectX::XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}}, // point at bottom-right
			{DirectX::XMFLOAT3{-0.5f, -0.5f,  0.0f}, DirectX::XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}}, // point at bottom-left
		};

		m_Scene.GetBuffer().Create(engine::VertexType::PosCol, vertex_data_array, 3);
	}

	void Controller::InitCamera()
	{
		int width = m_Window.GetClientWidth();
		int height = m_Window.GetClientHeight();

		m_Camera.SetPerspective(FOV, (float)width / (float)height, ZNEAR, ZFAR);
	}

	void Controller::MoveCamera(const DirectX::XMFLOAT3& offset, const DirectX::XMFLOAT3& angles)
	{
		m_Camera.AddRelativeOffset(offset);
		m_Camera.AddRelativeAngles(angles);
		m_Camera.UpdateMatrices();
	}

	void Controller::ProcessInput(float delta)
	{
		DirectX::XMFLOAT3 offset = { 0, 0, 0 };
		DirectX::XMFLOAT3 ang = { 0, 0, 0 };

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

			ang.x = (pos.x - m_LastXLB) * DirectX::XM_PI / 180.0f * m_RotationSpeed * delta;
			ang.y = (pos.y - m_LastYLB) * DirectX::XM_PI / 180.0f * m_RotationSpeed * delta;
		}
		else
		{
			m_FirstMoveLB = true;
		}

		MoveCamera(offset, ang);
	}

	LRESULT Controller::ProcessEvents(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
		{
			int width = m_Window.GetClientWidth();
			int height = m_Window.GetClientHeight();

			m_RotationSpeed = 360.0f / width;
			m_Camera.SetPerspective(FOV, (float)width / (float)height, ZNEAR, ZFAR);
			return 0;
		}
		case WM_KEYDOWN:
			m_Keys[wParam] = true;
			return 0;
		case WM_KEYUP:
			m_Keys[wParam] = false;
			return 0;
		case WM_LBUTTONDOWN:
			m_Keys[size_t(Keys::KEY_LMB)] = true;
			return 0;
		case WM_RBUTTONDOWN:
			m_Keys[size_t(Keys::KEY_RMB)] = true;
			return 0;
		case WM_LBUTTONUP:
			m_Keys[size_t(Keys::KEY_LMB)] = false;
			return 0;
		case WM_RBUTTONUP:
			m_Keys[size_t(Keys::KEY_RMB)] = false;
			return 0;
		case WM_MOUSEWHEEL:
			m_MouseWheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void Controller::Draw()
	{
		//m_Scene.Render(m_Window, m_Camera);
		float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_Window.ClearColor(color);
		m_Scene.Render(m_Window);

		m_Window.Flush();
	}

	WPARAM Controller::Run()
	{
		auto prevFrame = std::chrono::steady_clock::now();
		bool run = true;
		while (run)
		{
			run = m_Window.PoolEvents();

			auto currentFrame = std::chrono::steady_clock::now();
			auto delta = std::chrono::duration<float>(currentFrame - prevFrame).count();

			if (delta >= FRAME_DURATION)
			{
				prevFrame = currentFrame;
				ProcessInput(delta);
				Draw();
				std::cout << delta << std::endl;
			}

			std::this_thread::yield();
		}

		return 0;
	}
}