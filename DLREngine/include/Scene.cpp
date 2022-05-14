#include "Scene.h"

#include <memory>
#include <limits>

#include "math/SphereMover.h"
#include "math/TransformMover.h"
#include "math/Vector3Mover.h"

const float SHADOW_BIAS = 0.0005f;
const float AMBIENT_STRENGTH = 0.3f;
const float LIGHT_REP_RADIUS = 0.1f;

DirectX::XMVECTOR NdcToWorld(DirectX::XMVECTOR pos, const DirectX::XMMATRIX& mat) 
{
	pos = DirectX::XMVector4Transform(pos, mat);
	pos = DirectX::XMVectorScale(pos, 1.0f / DirectX::XMVectorGetW(pos));
	return pos;
}

bool Scene::Sphere::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	bool found = math::Sphere::Intersect(ray, outNearest);
	if (found)
	{
		outRef.type = IntersectedType::Sphere;
		outRef.object = this;
		outMaterial = &material;
	}
	return found;
}

bool Scene::Plane::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	bool found = math::Plane::Intersect(ray, outNearest);
	if (found)
	{
		outRef.type = IntersectedType::Plane;
		outRef.object = this;
		outMaterial = &material;
	}
	return found;
}

bool Scene::Transform::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	DirectX::XMMATRIX inv = math::Transform::ToInvMatrix();
	DirectX::XMVECTOR origin = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&ray.origin), 1.0f);
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&ray.direction);

	math::Ray modelRay;
	DirectX::XMStoreFloat3(&modelRay.origin, DirectX::XMVector4Transform(origin, inv));
	DirectX::XMStoreFloat3(&modelRay.direction, DirectX::XMVector4Transform(direction, inv));

	bool found = mesh.Intersect(modelRay, outNearest);
	if (found)
	{
		DirectX::XMStoreFloat3(&outNearest.pos, ray.PointAtLine(outNearest.t));
		outRef.type = IntersectedType::Transform;
		outRef.object = this;
		outMaterial = &material;
	}

	return found;
}

bool Scene::DirectionLight::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	math::Sphere light(direction, LIGHT_REP_RADIUS);
	bool found = light.Intersect(ray, outNearest);
	if (found)
	{
		outRef.type = IntersectedType::Light;
		outRef.object = this;
		outMaterial = &material;
	}
	return found;
}

DirectX::XMVECTOR Scene::DirectionLight::Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
	const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material)
{
	math::Intersection checkVisibility;
	checkVisibility.Reset();
	const math::Material* mPtr = nullptr;

	DirectX::XMVECTOR toLightDir = DirectX::XMVector3Normalize(DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&direction)));

	math::Ray pixelToLightRay;
	DirectX::XMStoreFloat3(&pixelToLightRay.origin, DirectX::XMVectorAdd(pixelPos, DirectX::XMVectorScale(toLightDir, SHADOW_BIAS)));
	DirectX::XMStoreFloat3(&pixelToLightRay.direction, toLightDir);

	bool visibility = scene.FindIntersection(pixelToLightRay, checkVisibility, mPtr, true);
	if (visibility)
		return DirectX::XMVectorZero();

	return math::DirectionLight::Illuminate(toLightDir, toCameraDir, pixelNormal, material);
}

bool Scene::PointLight::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	math::Sphere light(position, LIGHT_REP_RADIUS);
	bool found = light.Intersect(ray, outNearest);
	if (found)
	{
		outRef.type = IntersectedType::Light;
		outRef.object = this;
		outMaterial = &material;
	}
	return found;
}

DirectX::XMVECTOR Scene::PointLight::Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
	const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material)
{
	math::Intersection checkVisibility;
	checkVisibility.Reset();
	const math::Material* mPtr = nullptr;

	DirectX::XMVECTOR lightPos = DirectX::XMLoadFloat3(&position);

	DirectX::XMVECTOR toLightDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(lightPos, pixelPos));
	DirectX::XMVECTOR toLightDist = DirectX::XMVector3Length(DirectX::XMVectorSubtract(lightPos, pixelPos));

	math::Ray pixelToLightRay;
	DirectX::XMStoreFloat3(&pixelToLightRay.origin, DirectX::XMVectorAdd(pixelPos, DirectX::XMVectorScale(toLightDir, SHADOW_BIAS)));
	DirectX::XMStoreFloat3(&pixelToLightRay.direction, toLightDir);


	checkVisibility.t = DirectX::XMVectorGetX(toLightDist); // only check if something is between pixel and light
	bool visibility = scene.FindIntersection(pixelToLightRay, checkVisibility, mPtr, true);
	if (visibility)
		return DirectX::XMVectorZero();

	return math::PointLight::Illuminate(toLightDir, toLightDist, toCameraDir, pixelNormal, material);
}

bool Scene::SpotLight::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	math::Sphere light(position, LIGHT_REP_RADIUS);
	bool found = light.Intersect(ray, outNearest);
	if (found)
	{
		outRef.type = IntersectedType::Light;
		outRef.object = this;
		outMaterial = &material;
	}
	return found;
}

DirectX::XMVECTOR Scene::SpotLight::Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
	const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material)
{
	math::Intersection checkVisibility;
	checkVisibility.Reset();
	const math::Material* mPtr = nullptr;

	DirectX::XMVECTOR lightPos = DirectX::XMLoadFloat3(&position);

	DirectX::XMVECTOR toLightDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(lightPos, pixelPos));
	DirectX::XMVECTOR toLightDist = DirectX::XMVector3Length(DirectX::XMVectorSubtract(lightPos, pixelPos));

	math::Ray pixelToLightRay;
	DirectX::XMStoreFloat3(&pixelToLightRay.origin, DirectX::XMVectorAdd(pixelPos, DirectX::XMVectorScale(toLightDir, SHADOW_BIAS)));
	DirectX::XMStoreFloat3(&pixelToLightRay.direction, toLightDir);

	checkVisibility.t = DirectX::XMVectorGetX(toLightDist); // only check if something is between pixel and light
	bool visibility = scene.FindIntersection(pixelToLightRay, checkVisibility, mPtr, true);
	if (visibility)
		return DirectX::XMVectorZero();

	return math::SpotLight::Illuminate(toLightDir, toLightDist, toCameraDir, pixelNormal, material);
}

Scene::Scene()
{
}

void Scene::FindIntersectionInternal(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial, bool onlyObjects)
{
	for (auto& obj : m_Planes)
		obj.Intersect(ray, outRef, outNearest, outMaterial);

	for (auto& obj : m_Spheres)
		obj.Intersect(ray, outRef, outNearest, outMaterial);

	for (auto& obj : m_CubeTransforms)
		obj.Intersect(ray, outRef, outNearest, outMaterial);

	if (onlyObjects)
		return;

	for (auto& obj : m_DirLights)
		obj.Intersect(ray, outRef, outNearest, outMaterial);

	for (auto& obj : m_PointLights)
		obj.Intersect(ray, outRef, outNearest, outMaterial);

	for (auto& obj : m_SpotLights)
		obj.Intersect(ray, outRef, outNearest, outMaterial);
}

bool Scene::FindIntersection(const math::Ray& ray, math::Intersection& outNearest, const math::Material*& outMaterial, bool onlyObjects)
{
	ObjRef ref = { nullptr, IntersectedType::NUM };

	FindIntersectionInternal(ray, ref, outNearest, outMaterial, onlyObjects);

	return ref.type != IntersectedType::NUM;
}

bool Scene::FindIntersection(const math::Ray& ray, IntersectionQuery& query, bool onlyObjects)
{
	ObjRef ref = { nullptr, IntersectedType::NUM };

	FindIntersectionInternal(ray, ref, query.intersection, query.material, onlyObjects);

	switch (ref.type)
	{
	case IntersectedType::Sphere:
	{
		math::Sphere* sphere = static_cast<math::Sphere*>(ref.object);
		if (query.mover) query.mover->reset(new SphereMover(*sphere));
		break;
	}
	case IntersectedType::Transform:
	{
		math::Transform* transfrom = static_cast<math::Transform*>(ref.object);
		if (query.mover) query.mover->reset(new TransformMover(*transfrom));
		break;
	}
	case IntersectedType::Light:
	{
		// all light types have first member of XMFLOAT3 type, which is used for representing light position 
		DirectX::XMFLOAT3* vec = static_cast<DirectX::XMFLOAT3*>(ref.object);  
		if (query.mover) query.mover->reset(new Vector3Mover(*vec));
		break;
	}
	}

	return ref.type != IntersectedType::NUM;
}

bool Scene::Render(MainWindow& win, Camera& camera)
{
	int width = win.GetClientWidth();
	int height = win.GetClientHeight();
	
	std::vector<int32_t>& pixels = win.GetPixels();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float xNDC = (2.0f * x / width) - 1.0f;
			float yNDC = (2.0f * y / height) - 1.0f;
			
			DirectX::XMVECTOR cameraPos = camera.Position();
			DirectX::XMVECTOR worldPos = NdcToWorld(DirectX::XMVectorSet(xNDC, yNDC, 1.0f, 1.0f), camera.GetInvViewProj());
			DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(worldPos, cameraPos));

			DirectX::XMFLOAT3 d, p;
			DirectX::XMStoreFloat3(&d, direction);
			DirectX::XMStoreFloat3(&p, cameraPos);

			math::Ray r(p, d);
			DirectX::XMFLOAT3 col = ComputeColor(r, camera.Position());

			int index = y * width + x;
			pixels[index] = (int)(col.x * 255.9f) << 16;
			pixels[index] |= (int)(col.y * 255.9f) << 8;
			pixels[index] |= (int)(col.z * 255.9f) << 0;
		}
	}
	return true;
}

DirectX::XMFLOAT3 Scene::ComputeColor(const math::Ray& castedRay, const DirectX::XMVECTOR& cameraPos)
{
	math::Intersection rec;
	rec.Reset();

	ObjRef ref = { nullptr, IntersectedType::NUM };
	const math::Material* materialPtr = nullptr;
	
	FindIntersectionInternal(castedRay, ref, rec, materialPtr, false);

	if (ref.type == IntersectedType::Light)
		return materialPtr->albedo;

	if (ref.type != IntersectedType::NUM)
	{
		DirectX::XMFLOAT3 color;
		DirectX::XMVECTOR pixelPos, pixelNormal, pixelToCamera, sum;
		math::MaterialVectorized matVec = materialPtr->Vectorize();

		pixelPos = DirectX::XMLoadFloat3(&rec.pos);
		pixelNormal = DirectX::XMLoadFloat3(&rec.normal);
		pixelToCamera = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(cameraPos, pixelPos));

		sum = DirectX::XMVectorMultiply(matVec.albedo, DirectX::XMVectorReplicate(AMBIENT_STRENGTH));
		sum = DirectX::XMVectorAdd(sum, matVec.emission);

		for (auto& dirLight : m_DirLights)
			sum = DirectX::XMVectorAdd(sum, dirLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, matVec));

		for (auto& pointLight : m_PointLights)
			sum = DirectX::XMVectorAdd(sum, pointLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, matVec));

		for (auto& spotLight : m_SpotLights)
			sum = DirectX::XMVectorAdd(sum, spotLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, matVec));

		sum = DirectX::XMVectorClamp(sum, DirectX::XMVectorZero(), DirectX::XMVectorReplicate(1.0f));
		DirectX::XMStoreFloat3(&color, sum);
		
		return color;
	}

	DirectX::XMFLOAT3 unit;
	DirectX::XMStoreFloat3(&unit, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&castedRay.direction)));
	float t = 0.5 * (unit.y + 1.0);
	float it = 1.0 - t;

	float r = it + t * 0.5f;
	float g = it + t * 0.7f;
	float b = it + t * 1.0f;
	return DirectX::XMFLOAT3(r, g, b);
}
