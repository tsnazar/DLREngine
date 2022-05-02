#include "Scene.h"

#include <memory>
#include "../math/sphere.h"
#include "../math/mat4.h"
#include "../math/mat4.h"

#define VK_A 0x41
#define VK_D 0x44
#define VK_S 0x53
#define VK_W 0x57
#define VK_PRESSED 0x8000

const float VELOCITY = 100.0f;

Scene::Scene()
{
	m_BMI.bmiHeader.biSize = sizeof(m_BMI);
	m_BMI.bmiHeader.biWidth = 0;
	m_BMI.bmiHeader.biHeight = 0;
	m_BMI.bmiHeader.biPlanes = 1;
	m_BMI.bmiHeader.biBitCount = 32;
	m_BMI.bmiHeader.biCompression = BI_RGB;

	m_Objects.push_back(std::make_unique<math::sphere>(math::vec3(50, 50, -1), 50));
}

bool Scene::Render(const MainWindow& win)
{
	int width = win.GetClientWidth();
	int height = win.GetClientHeight();

	if (m_BMI.bmiHeader.biWidth != width || m_BMI.bmiHeader.biHeight != height)
	{
		m_BMI.bmiHeader.biWidth = width;
		m_BMI.bmiHeader.biHeight = height;
		m_Pixels.resize(width * height);
	}

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			math::ray r(math::vec3(x, y, 0) - m_Offsets, math::vec3(0, 0, -1));
			math::vec3 col = 255 * Color(r);

			int index = y * width + x;
			m_Pixels[index] = (int)col.x << 16;
			m_Pixels[index] |= (int)col.y << 8;
			m_Pixels[index] |= (int)col.z << 0;
		}
	}

	SetDIBitsToDevice(win.GetHDC(),
		0,
		0,
		width,
		height,
		0,
		0,
		0,
		height,
		m_Pixels.data(),
		&m_BMI,
		DIB_RGB_COLORS
	);

	return true;
}

bool Scene::ProcessInput(float delta)
{
	static bool firstMove = true;

	if (GetKeyState(VK_A) & VK_PRESSED)
		m_Offsets.x -= VELOCITY * delta;
	if (GetKeyState(VK_D) & VK_PRESSED)
		m_Offsets.x += VELOCITY * delta;
	if (GetKeyState(VK_S) & VK_PRESSED)
		m_Offsets.y -= VELOCITY * delta;
	if (GetKeyState(VK_W) & VK_PRESSED)
		m_Offsets.y += VELOCITY * delta;

	if (GetKeyState(VK_RBUTTON) & VK_PRESSED)
	{
		static int lastX, lastY;

		POINT pos;
		GetCursorPos(&pos);
		int x = pos.x;
		int y = pos.y;

		if (firstMove)
		{
			firstMove = false;
			lastX = x;
			lastY = y;
		}

		math::vec3 p(x - lastX, lastY - y, 0);
		m_Offsets += p;

		lastX = x;
		lastY = y;
	}
	else
	{
		firstMove = true;
	}

	return true;
}

math::vec3 Scene::Color(const math::ray& castedRay)
{
	math::hit_record rec;
	if (m_Objects.hit(castedRay, 0.0, 1000, rec))
	{
		return math::vec3(1, 0, 0);
	}
	math::vec3 unit = math::normalize(castedRay.direction);
	float t = 0.5 * (unit.y + 1.0);
	return (1.0 - t) * math::vec3(1.0, 1.0, 1.0) + t * math::vec3(0.5, 0.7, 1.0);
}
