#include "Scene.h"

#include <memory>
#include <limits>
#include <algorithm>

#include "math/SphereMover.h"
#include "math/TransformMover.h"
#include "math/Vector3Mover.h"

using namespace DirectX;

namespace
{
	const float SHADOW_BIAS = 0.0005f;
	const float MIRROR_BIAS = 0.0005f;
	const float LIGHT_REP_RADIUS = 0.1f;
	const float SCENE_REFLECTION_MAX_ROUGHNESS = 0.1f;
	const float LIGHT_COLOR_STRENGTH = 10.0f;
	const int MAX_DEPTH = 20;
	const XMVECTOR GAMMA_CORRECTION = XMVectorReplicate(1.0f / 2.2f);
}

static XMVECTOR findMaxComponent(const XMVECTOR& vec)
{
	return XMVectorReplicate((std::max)((std::max)(XMVectorGetX(vec), XMVectorGetY(vec)), XMVectorGetZ(vec)));
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

bool Scene::MeshInstance::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	XMMATRIX inv = transform.ToInvMatrix();
	XMVECTOR origin = XMVectorSetW(XMLoadFloat3(&ray.origin), 1.0f);
	XMVECTOR direction = XMLoadFloat3(&ray.direction);

	math::Ray modelRay;
	XMStoreFloat3(&modelRay.origin, XMVector4Transform(origin, inv));
	XMStoreFloat3(&modelRay.direction, XMVector4Transform(direction, inv));

	bool found = mesh->Intersect(modelRay, outNearest);
	if (found)
	{
		XMStoreFloat3(&outNearest.pos, ray.PointAtLine(outNearest.t));
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

XMVECTOR Scene::DirectionLight::Illuminate(Scene& scene, const XMVECTOR& toCameraDir, const XMVECTOR& pixelPos,
	const XMVECTOR& pixelNormal, const XMVECTOR& NdotV,  const math::MaterialVectorized& material)
{
	math::Intersection checkVisibility;
	checkVisibility.Reset();
	const math::Material* mPtr = nullptr;

	XMVECTOR toLightDir = XMVector3Normalize(XMVectorNegate(XMLoadFloat3(&direction)));

	math::Ray pixelToLightRay;
	XMStoreFloat3(&pixelToLightRay.origin, XMVectorAdd(pixelPos, XMVectorScale(toLightDir, SHADOW_BIAS)));
	XMStoreFloat3(&pixelToLightRay.direction, toLightDir);

	bool visibility = scene.FindIntersection(pixelToLightRay, checkVisibility, mPtr, true);
	if (visibility)
		return XMVectorZero();

	return math::DirectionLight::Illuminate(toLightDir, toCameraDir, pixelNormal, NdotV, material);
}

bool Scene::PointLight::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	math::Sphere light(position, radius);
	bool found = light.Intersect(ray, outNearest);
	if (found)
	{
		outRef.type = IntersectedType::Light;
		outRef.object = this;
		outMaterial = &material;
	}
	return found;
}

XMVECTOR Scene::PointLight::Illuminate(Scene& scene, const XMVECTOR& toCameraDir, const XMVECTOR& pixelPos,
	const XMVECTOR& pixelNormal, const XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	math::Intersection checkVisibility;
	checkVisibility.Reset();
	const math::Material* mPtr = nullptr;

	XMVECTOR lightPos = XMLoadFloat3(&position);

	XMVECTOR toLightDir = XMVector3Normalize(XMVectorSubtract(lightPos, pixelPos));
	XMVECTOR toLightDist = XMVector3Length(XMVectorSubtract(lightPos, pixelPos));

	math::Ray pixelToLightRay;
	XMStoreFloat3(&pixelToLightRay.origin, XMVectorAdd(pixelPos, XMVectorScale(toLightDir, SHADOW_BIAS)));
	XMStoreFloat3(&pixelToLightRay.direction, toLightDir);

	checkVisibility.t = XMVectorGetX(toLightDist); // only check if something is between pixel and light
	bool visibility = scene.FindIntersection(pixelToLightRay, checkVisibility, mPtr, true);
	if (visibility)
		return XMVectorZero();

	return math::PointLight::Illuminate(toLightDir, toLightDist, toCameraDir, pixelNormal, NdotV, material);
}

bool Scene::SpotLight::Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial)
{
	math::Sphere light(position, radius);
	bool found = light.Intersect(ray, outNearest);
	if (found)
	{
		outRef.type = IntersectedType::Light;
		outRef.object = this;
		outMaterial = &material;
	}
	return found;
}

XMVECTOR Scene::SpotLight::Illuminate(Scene& scene, const XMVECTOR& toCameraDir, const XMVECTOR& pixelPos,
	const XMVECTOR& pixelNormal, const XMVECTOR& NdotV, const math::MaterialVectorized& material)
{
	math::Intersection checkVisibility;
	checkVisibility.Reset();
	const math::Material* mPtr = nullptr;

	XMVECTOR lightPos = XMLoadFloat3(&position);

	XMVECTOR toLightDir = XMVector3Normalize(XMVectorSubtract(lightPos, pixelPos));
	XMVECTOR toLightDist = XMVector3Length(XMVectorSubtract(lightPos, pixelPos));

	math::Ray pixelToLightRay;
	XMStoreFloat3(&pixelToLightRay.origin, XMVectorAdd(pixelPos, XMVectorScale(toLightDir, SHADOW_BIAS)));
	XMStoreFloat3(&pixelToLightRay.direction, toLightDir);

	checkVisibility.t = XMVectorGetX(toLightDist); // only check if something is between pixel and light
	bool visibility = scene.FindIntersection(pixelToLightRay, checkVisibility, mPtr, true);
	if (visibility)
		return XMVectorZero();

	return math::SpotLight::Illuminate(toLightDir, toLightDist, toCameraDir, pixelNormal, NdotV, material);
}

void Scene::FindIntersectionInternal(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial, bool onlyObjects)
{
	for (auto& obj : m_Planes)
		obj.Intersect(ray, outRef, outNearest, outMaterial);

	for (auto& obj : m_Spheres)
		obj.Intersect(ray, outRef, outNearest, outMaterial);

	for (auto& obj : m_Meshes)
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
		XMFLOAT3* vec = static_cast<XMFLOAT3*>(ref.object);  
		if (query.mover) query.mover->reset(new Vector3Mover(*vec));
		break;
	}
	}

	return ref.type != IntersectedType::NUM;
}

bool Scene::Render(MainWindow& win, Camera& camera)
{
	if (m_GlobalIllumination && m_Rendered)
		return true;

	int width = win.GetImageWidth();
	int height = win.GetImageHeight();
	
	std::vector<int32_t>& pixels = win.GetPixels();

	XMVECTOR bottomLeft = camera.Unproject(XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f));
	XMVECTOR bottomRight = camera.Unproject(XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f));
	XMVECTOR topLeft = camera.Unproject(XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f));
	XMVECTOR xDir = XMVectorSubtract(bottomRight, bottomLeft);
	XMVECTOR yDir = XMVectorSubtract(topLeft, bottomLeft);
	XMVECTOR cameraPos = camera.Position();

	auto func = [&](uint32_t threadIndex, uint32_t taskIndex) {
		int x = taskIndex % width;
		int y = taskIndex / width;
		float xNDC = (x + 0.5f) / width;
		float yNDC = (y + 0.5f) / height;

		XMVECTOR worldPos = XMVectorAdd(bottomLeft, XMVectorAdd(XMVectorScale(xDir, xNDC), XMVectorScale(yDir, yNDC))); //BL + xNDC * (BR-BL) + yNDC * (TL-BL)
		XMVECTOR direction = XMVector3Normalize(XMVectorSubtract(worldPos, cameraPos));

		XMFLOAT3 d, p;
		XMStoreFloat3(&d, direction);
		XMStoreFloat3(&p, cameraPos);

		math::Ray r(p, d);
		XMVECTOR col = ComputeLighting(r, camera.Position(), MAX_DEPTH);
		col = math::adjustExposure(col, m_EV100);
		col = math::acesHDRtoLDR(col);
		XMVectorPow(col, GAMMA_CORRECTION);

		XMFLOAT3 color;
		XMStoreFloat3(&color, col);

		int index = y * width + x;
		pixels[index] = (int)(color.x * 255.9f) << 16;
		pixels[index] |= (int)(color.y * 255.9f) << 8;
		pixels[index] |= (int)(color.z * 255.9f) << 0;
	};

	m_Executor.Execute(func, width * height, 20);
	m_Rendered = true;

	return true;
}

XMVECTOR Scene::ComputeLighting(const math::Ray& ray, const XMVECTOR& cameraPos, int depth)
{
	if (depth == 0)
		return XMVectorZero();

	math::Intersection rec;
	rec.Reset();

	ObjRef ref = { nullptr, IntersectedType::NUM };
	const math::Material* materialPtr = nullptr;
	
	if(depth != MAX_DEPTH)
		FindIntersectionInternal(ray, ref, rec, materialPtr, true);
	else
		FindIntersectionInternal(ray, ref, rec, materialPtr, false);

	if (ref.type == IntersectedType::Light)
	{
		XMVECTOR lightColor = XMLoadFloat3(&materialPtr->emission);
		return XMVectorScale(XMVectorDivide(lightColor, findMaxComponent(lightColor)), LIGHT_COLOR_STRENGTH);
	}

	if (ref.type != IntersectedType::NUM)
	{
		XMVECTOR pixelPos, pixelNormal, pixelToCamera, NdotV, sum, ambient;
		math::MaterialVectorized matVec = materialPtr->Vectorize();

		pixelPos = XMLoadFloat3(&rec.pos);
		pixelNormal = XMLoadFloat3(&rec.normal);
		pixelToCamera = XMVectorNegate(XMLoadFloat3(&ray.direction));
		NdotV = XMVectorMax(XMVector3Dot(pixelNormal, pixelToCamera), XMVectorZero());
		
		ambient = (m_GlobalIllumination && depth == MAX_DEPTH) ? CalculateGlobal(pixelNormal, pixelPos, cameraPos) : CalculateAmbient(pixelNormal);

		sum = XMVectorMultiply(matVec.albedo, ambient);
		sum = XMVectorMultiply(sum, XMVectorSubtract(XMVectorReplicate(1.0f), matVec.metalic));
		sum = XMVectorAdd(sum, matVec.emission);

		for (auto& dirLight : m_DirLights)
			sum = XMVectorAdd(sum, dirLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, NdotV, matVec));

		for (auto& pointLight : m_PointLights)
			sum = XMVectorAdd(sum, pointLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, NdotV, matVec));

		for (auto& spotLight : m_SpotLights)
			sum = XMVectorAdd(sum, spotLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, NdotV, matVec));

		if (m_Reflections && materialPtr->roughness < SCENE_REFLECTION_MAX_ROUGHNESS)
		{
			// R = - V + 2.0 * N * NdotV;
			XMVECTOR reflection = XMVectorAdd(XMVectorNegate(pixelToCamera), XMVectorScale(XMVectorMultiply(pixelNormal, NdotV), 2.0f));
			
			math::Ray reflectanceRay;
			XMStoreFloat3(&reflectanceRay.origin, XMVectorAdd(pixelPos, XMVectorScale(reflection, MIRROR_BIAS)));
			XMStoreFloat3(&reflectanceRay.direction, reflection);

			XMVECTOR addEnergy = ComputeLighting(reflectanceRay, cameraPos, depth - 1);
			addEnergy = XMVectorMultiply(addEnergy, math::fresnel(matVec.f0, XMVector3Dot(reflection, pixelNormal)));

			float sceneReflectionFading = 1.f - (std::min)(1.f, materialPtr->roughness / SCENE_REFLECTION_MAX_ROUGHNESS);
			addEnergy = XMVectorMultiply(addEnergy, XMVectorReplicate(sceneReflectionFading));

			sum = XMVectorAdd(sum, addEnergy);
		}

		return sum;
	}

	return CalculateAmbient(XMVector3Normalize(XMLoadFloat3(&ray.direction)));
}

XMVECTOR Scene::CalculateAmbient(const XMVECTOR& dir)
{
	using namespace DirectX;
	XMVECTOR t = XMVectorReplicate(0.5f) * (XMVectorSplatY(dir) + XMVectorReplicate(1.0f));
	XMVECTOR it = XMVectorReplicate(1.0f) - t;
	return it + (t * XMVectorSet(0.5f, 0.7f, 1.0f, 0.0f));
}

XMVECTOR Scene::CalculateGlobal(const XMVECTOR& dir, const XMVECTOR& pos, const XMVECTOR& cameraPos)
{
	XMVECTOR b1, b2;
	XMMATRIX transform;

	XMVECTOR ambient = XMVectorZero();

	math::branchlessONB(dir, b1, b2);

	transform.r[0] = b1;
	transform.r[1] = b2;
	transform.r[2] = dir;
	transform.r[3] = XMVectorSet(0, 0, 0, 1);

	for (unsigned int i = 0; i < m_HemisphereSamples.size(); ++i)
	{
		XMVECTOR rayDir = XMVector3Normalize(XMVector3Transform(m_HemisphereSamples[i], transform));

		math::Ray ray;
		XMStoreFloat3(&ray.origin, XMVectorAdd(pos, XMVectorScale(rayDir, MIRROR_BIAS)));
		XMStoreFloat3(&ray.direction, rayDir);

		ambient = XMVectorAdd(ambient, ComputeLighting(ray, cameraPos, MAX_DEPTH - 1));
	}

	return XMVectorScale(ambient, XM_2PI / m_HemisphereSamples.size());
}