#include "Scene.h"

#include <memory>
#include "../math/sphere.h"
#include "../math/mat4.h"
#include "../math/mat4.h"

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
			math::ray r(math::vec3(x, y, 0) - m_Offsets, math::vec3(0, 0, -1));
			math::vec3 col = 255 * ComputeColor(r);

			int index = y * width + x;
			pixels[index] = (int)col.x << 16;
			pixels[index] |= (int)col.y << 8;
			pixels[index] |= (int)col.z << 0;
		}
	}
	return true;
}

math::vec3 Scene::ComputeColor(const math::ray& castedRay)
{
	math::hit_record rec;
	for (const auto& obj : m_Objects)
		if (obj.hit(castedRay, 0, 100, rec))
			return math::vec3(1, 0, 0);

	math::vec3 unit = math::normalize(castedRay.direction);
	float t = 0.5 * (unit.y + 1.0);
	return (1.0 - t) * math::vec3(1.0, 1.0, 1.0) + t * math::vec3(0.5, 0.7, 1.0);
}
