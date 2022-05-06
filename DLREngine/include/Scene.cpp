#include "Scene.h"

#include <memory>
#include <limits>
#include "../math/sphere.h"
//#include "../math/mat4.h"
//#include "../math/mat4.h"

Scene::Scene()
{
}

bool Scene::Render(MainWindow& win)
{
	int width = win.GetClientWidth();
	int height = win.GetClientHeight();
	
	std::vector<int32_t>& pixels = win.GetPixels();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			math::ray r(DirectX::XMFLOAT3(x - m_Offset.x, y - m_Offset.y, 0 - m_Offset.z), DirectX::XMFLOAT3(0, 0, -1));
			DirectX::XMFLOAT3 col = ComputeColor(r);

			int index = y * width + x;
			pixels[index] = (int)(col.x * 255.9f) << 16;
			pixels[index] |= (int)(col.y * 255.9f) << 8;
			pixels[index] |= (int)(col.z * 255.9f) << 0;
		}
	}
	return true;
}

DirectX::XMFLOAT3 Scene::ComputeColor(const math::ray& castedRay)
{
	math::hit_record rec;
	for (const auto& obj : m_Objects)
		if (obj.hit(castedRay, 0, std::numeric_limits<float>::infinity(), rec))
			return DirectX::XMFLOAT3(1, 0, 0);

	DirectX::XMFLOAT3 unit;
	DirectX::XMStoreFloat3(&unit, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&castedRay.direction)));
	float t = 0.5 * (unit.y + 1.0);
	float it = 1.0 - t;

	float r = it + t * 0.5f;
	float g = it + t * 0.7f;
	float b = it + t * 1.0f;
	return DirectX::XMFLOAT3(r, g, b);
}
