#pragma once

#include "Camera.h"
#include "Event.h"
#include "MouseEvents.h"
#include "ApplicationEvent.h"
#include "Ray.h"

namespace engine
{

	class CameraController
	{
	public:
		CameraController(float fov, float aspect, float near, float far);

		void OnUpdate(float delta);
		void OnEvent(Event& e);

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		Ray GetPickingRay();

		Camera& GetCamera() { return m_Camera; }
		const Camera& GetCamera() const { return m_Camera; }

	private:
		Camera m_Camera;

		float m_FOV, m_ZNear, m_ZFar;

		int m_MouseWheelDelta = 0;
		float m_CameraVelocity = 5.0f;
		float m_RotationSpeed;

		bool m_FirstMoveLB = true;
		int m_LastXLB = 0, m_LastYLB = 0;
	};
}
