#include "Scene.h"

#include <memory>
#include <limits>
#include "../math/sphere.h"

static DirectX::XMVECTOR NdcToWorld(DirectX::XMVECTOR pos, const DirectX::XMMATRIX& mat) 
{
	pos = DirectX::XMVector4Transform(pos, mat);
	pos = DirectX::XMVectorScale(pos, DirectX::XMVectorGetW(pos));
	return pos;
}

Scene::Scene()
{
}

bool Scene::Render(MainWindow& win, Camera& camera)
{
	int width = win.GetClientWidth();
	int height = win.GetClientHeight();
	
	camera.SetPerspective(0.5f, (float)width / (float)height,  0.1f, 100.0f);

	std::vector<int32_t>& pixels = win.GetPixels();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float xNDC = (2.0f * x / width) - 1.0f;
			float yNDC = (2.0f * y / height) - 1.0f;
			
			DirectX::XMVECTOR pos = camera.Position();
			DirectX::XMVECTOR dir = NdcToWorld(DirectX::XMVectorSet(xNDC, yNDC, 1.0f, 1.0f), camera.GetInvViewProj());

			DirectX::XMFLOAT3 d, p;
			DirectX::XMStoreFloat3(&d, dir);
			DirectX::XMStoreFloat3(&p, pos);

			math::ray r(p, d);
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
	for (const auto& obj : m_Spheres)
		if (obj.hit(castedRay, 0, std::numeric_limits<float>::infinity(), rec))
			return  DirectX::XMFLOAT3(0.5 * (rec.normal.x + 1), 0.5 * (rec.normal.y + 1), 0.5 * (rec.normal.z + 1));
	
	for(const auto& obj: m_Planes)
		if(obj.hit(castedRay, 0, std::numeric_limits<float>::infinity(), rec))
			return DirectX::XMFLOAT3(0, 1, 0);

	DirectX::XMFLOAT3 unit;
	DirectX::XMStoreFloat3(&unit, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&castedRay.direction)));
	float t = 0.5 * (unit.y + 1.0);
	float it = 1.0 - t;

	float r = it + t * 0.5f;
	float g = it + t * 0.7f;
	float b = it + t * 1.0f;
	return DirectX::XMFLOAT3(r, g, b);
}
