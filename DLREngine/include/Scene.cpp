#include "Scene.h"

#include <memory>
#include <limits>
#include <algorithm>

#include "math/SphereMover.h"
#include "math/TransformMover.h"
#include "math/Vector3Mover.h"
#include "lights/BRDF.h"

namespace
{
	const float SHADOW_BIAS = 0.0005f;
	const float MIRROR_BIAS = 0.0005f;
	const float LIGHT_REP_RADIUS = 0.1f;
	const float SCENE_REFLECTION_MAX_ROUGHNESS = 0.1f;
	const float LIGHT_COLOR_STRENGTH = 10.0f;
	const int MAX_DEPTH = 20;
	const DirectX::XMVECTOR GAMMA_CORRECTION = DirectX::XMVectorReplicate(1.0f / 2.2f);
}

static DirectX::XMVECTOR findMaxComponent(const DirectX::XMVECTOR& vec)
{
	return DirectX::XMVectorReplicate((std::max)((std::max)(DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec)), DirectX::XMVectorGetZ(vec)));
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
	DirectX::XMMATRIX inv = transform.ToInvMatrix();
	DirectX::XMVECTOR origin = DirectX::XMVectorSetW(DirectX::XMLoadFloat3(&ray.origin), 1.0f);
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&ray.direction);

	math::Ray modelRay;
	DirectX::XMStoreFloat3(&modelRay.origin, DirectX::XMVector4Transform(origin, inv));
	DirectX::XMStoreFloat3(&modelRay.direction, DirectX::XMVector4Transform(direction, inv));

	bool found = mesh->Intersect(modelRay, outNearest);
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
	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV,  const math::MaterialVectorized& material)
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

DirectX::XMVECTOR Scene::PointLight::Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material)
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

	// (1-cos(2*asin(radius/distance_to_light)))
	DirectX::XMVECTOR solidAngle = DirectX::XMVectorSubtract(DirectX::XMVectorReplicate(1.0f),
		DirectX::XMVectorCos(DirectX::XMVectorScale(DirectX::XMVectorASin(DirectX::XMVectorDivide(DirectX::XMVectorReplicate(radius), toLightDist)), 2.0f))); 

	return DirectX::XMVectorMultiply(math::PointLight::Illuminate(toLightDir, toLightDist, toCameraDir, pixelNormal, NdotV, material), solidAngle);
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

DirectX::XMVECTOR Scene::SpotLight::Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
	const DirectX::XMVECTOR& pixelNormal, const DirectX::XMVECTOR& NdotV, const math::MaterialVectorized& material)
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

	// (1-cos(2*asin(radius/distance_to_light)))
	DirectX::XMVECTOR solidAngle = DirectX::XMVectorSubtract(DirectX::XMVectorReplicate(1.0f), 
		DirectX::XMVectorCos(DirectX::XMVectorScale(DirectX::XMVectorASin(DirectX::XMVectorDivide(DirectX::XMVectorReplicate(radius), toLightDist)), 2.0f)));

	return DirectX::XMVectorMultiply(math::SpotLight::Illuminate(toLightDir, toLightDist, toCameraDir, pixelNormal, NdotV, material), solidAngle);
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
		DirectX::XMFLOAT3* vec = static_cast<DirectX::XMFLOAT3*>(ref.object);  
		if (query.mover) query.mover->reset(new Vector3Mover(*vec));
		break;
	}
	}

	return ref.type != IntersectedType::NUM;
}

bool Scene::Render(MainWindow& win, Camera& camera)
{
	int width = win.GetImageWidth();
	int height = win.GetImageHeight();
	
	std::vector<int32_t>& pixels = win.GetPixels();

	DirectX::XMVECTOR bottomLeft = camera.Unproject(DirectX::XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f));
	DirectX::XMVECTOR bottomRight = camera.Unproject(DirectX::XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f));
	DirectX::XMVECTOR topLeft = camera.Unproject(DirectX::XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f));
	DirectX::XMVECTOR xDir = DirectX::XMVectorSubtract(bottomRight, bottomLeft);
	DirectX::XMVECTOR yDir = DirectX::XMVectorSubtract(topLeft, bottomLeft);
	DirectX::XMVECTOR cameraPos = camera.Position();

	auto func = [&](uint32_t threadIndex, uint32_t taskIndex) {
		int x = taskIndex % width;
		int y = taskIndex / width;
		float xNDC = (x + 0.5f) / width;
		float yNDC = (y + 0.5f) / height;

		DirectX::XMVECTOR worldPos = DirectX::XMVectorAdd(bottomLeft, DirectX::XMVectorAdd(DirectX::XMVectorScale(xDir, xNDC), DirectX::XMVectorScale(yDir, yNDC))); //BL + xNDC * (BR-BL) + yNDC * (TL-BL)
		DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(worldPos, cameraPos));

		DirectX::XMFLOAT3 d, p;
		DirectX::XMStoreFloat3(&d, direction);
		DirectX::XMStoreFloat3(&p, cameraPos);

		math::Ray r(p, d);
		DirectX::XMVECTOR col = ComputeLighting(r, camera.Position(), MAX_DEPTH);
		col = AdjustExposure(col, m_EV100);
		col = AcesHDRtoLDR(col);
		DirectX::XMVectorPow(col, GAMMA_CORRECTION);

		DirectX::XMFLOAT3 color;
		DirectX::XMStoreFloat3(&color, col);

		int index = y * width + x;
		pixels[index] = (int)(color.x * 255.9f) << 16;
		pixels[index] |= (int)(color.y * 255.9f) << 8;
		pixels[index] |= (int)(color.z * 255.9f) << 0;
	};

	m_Executor.Execute(func, width * height, 20);

	return true;
}

DirectX::XMVECTOR Scene::ComputeLighting(const math::Ray& ray, const DirectX::XMVECTOR& cameraPos, int depth)
{
	if (depth == 0)
		return DirectX::XMVectorZero();

	math::Intersection rec;
	rec.Reset();

	ObjRef ref = { nullptr, IntersectedType::NUM };
	const math::Material* materialPtr = nullptr;
	
	FindIntersectionInternal(ray, ref, rec, materialPtr, false);

	if (ref.type == IntersectedType::Light)
	{
		DirectX::XMVECTOR lightColor = DirectX::XMLoadFloat3(&materialPtr->albedo);
		return DirectX::XMVectorScale(DirectX::XMVectorDivide(lightColor, findMaxComponent(lightColor)), LIGHT_COLOR_STRENGTH);
	}

	DirectX::XMFLOAT3 unit;
	DirectX::XMStoreFloat3(&unit, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&ray.direction)));
	float t = 0.5 * (unit.y + 1.0);
	float it = 1.0 - t;

	float r = it + t * 0.5f;
	float g = it + t * 0.7f;
	float b = it + t * 1.0f;
	DirectX::XMVECTOR ambient = DirectX::XMVectorSet(r, g, b, 0);

	if (ref.type != IntersectedType::NUM)
	{
		DirectX::XMVECTOR pixelPos, pixelNormal, pixelToCamera, NdotV, sum;
		math::MaterialVectorized matVec = materialPtr->Vectorize();

		matVec.f0 = math::mix(matVec.f0, matVec.albedo, matVec.metalic);

		pixelPos = DirectX::XMLoadFloat3(&rec.pos);
		pixelNormal = DirectX::XMLoadFloat3(&rec.normal);
		pixelToCamera = DirectX::XMVectorNegate(DirectX::XMLoadFloat3(&ray.direction));
		NdotV = DirectX::XMVectorMax(DirectX::XMVector3Dot(pixelNormal, pixelToCamera), DirectX::XMVectorZero());

		sum = DirectX::XMVectorMultiply(matVec.albedo, ambient);
		sum = DirectX::XMVectorMultiply(sum, DirectX::XMVectorSubtract(DirectX::XMVectorReplicate(1.0f), matVec.metalic));
		sum = DirectX::XMVectorAdd(sum, matVec.emission);

		for (auto& dirLight : m_DirLights)
			sum = DirectX::XMVectorAdd(sum, dirLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, NdotV, matVec));

		for (auto& pointLight : m_PointLights)
			sum = DirectX::XMVectorAdd(sum, pointLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, NdotV, matVec));

		for (auto& spotLight : m_SpotLights)
			sum = DirectX::XMVectorAdd(sum, spotLight.Illuminate(*this, pixelToCamera, pixelPos, pixelNormal, NdotV, matVec));

		if (m_Reflections && materialPtr->roughness < SCENE_REFLECTION_MAX_ROUGHNESS)
		{
			// R = - V + 2.0 * N * NdotV;
			DirectX::XMVECTOR reflection = DirectX::XMVectorAdd(DirectX::XMVectorNegate(pixelToCamera), DirectX::XMVectorScale(DirectX::XMVectorMultiply(pixelNormal, NdotV), 2.0f));
			
			math::Ray reflectanceRay;
			DirectX::XMStoreFloat3(&reflectanceRay.origin, DirectX::XMVectorAdd(pixelPos, DirectX::XMVectorScale(reflection, MIRROR_BIAS)));
			DirectX::XMStoreFloat3(&reflectanceRay.direction, reflection);

			DirectX::XMVECTOR addEnergy = ComputeLighting(reflectanceRay, cameraPos, depth - 1);
			addEnergy = DirectX::XMVectorMultiply(addEnergy, math::fresnel(matVec.f0, DirectX::XMVector3Dot(reflection, pixelNormal)));

			float sceneReflectionFading = 1.f - (std::min)(1.f, materialPtr->roughness / SCENE_REFLECTION_MAX_ROUGHNESS);
			addEnergy = DirectX::XMVectorMultiply(addEnergy, DirectX::XMVectorReplicate(sceneReflectionFading));

			sum = DirectX::XMVectorAdd(sum, addEnergy);
		}

		return sum;
	}

	return ambient;
}

DirectX::XMVECTOR Scene::AdjustExposure(const DirectX::XMVECTOR& color, float EV100)
{
	float LMax = (78.0f / (0.65f * 100.0f)) * powf(2.0f, EV100);
	return DirectX::XMVectorMultiply(color, DirectX::XMVectorReplicate(1.0f / LMax));
}

DirectX::XMVECTOR Scene::AcesHDRtoLDR(const DirectX::XMVECTOR& hdr)
{
	const DirectX::XMMATRIX m1 = DirectX::XMMatrixSet(0.59719f, 0.07600f, 0.02840f, 0.0f, 0.35458f, 0.90834f, 0.13383f, 0.0f, 0.04823f, 0.01566f, 0.83777f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMMATRIX m2 = DirectX::XMMatrixSet(1.60475f, -0.10208, -0.00327f, 0.0f, -0.53108f, 1.10813, -0.07276f, 0.0f, -0.07367f, -0.00605, 1.07602f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR v = DirectX::XMVector3Transform(hdr, m1);
	DirectX::XMVECTOR a = DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(v, DirectX::XMVectorAdd(v, DirectX::XMVectorReplicate(0.0245786f))), DirectX::XMVectorReplicate(0.000090537f));
	DirectX::XMVECTOR b = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(v, DirectX::XMVectorAdd(DirectX::XMVectorMultiply(v, DirectX::XMVectorReplicate(0.983729f)), 
		DirectX::XMVectorReplicate(0.4329510f))), DirectX::XMVectorReplicate(0.238081f));
	DirectX::XMVECTOR ldr = DirectX::XMVectorClamp(DirectX::XMVector3Transform(DirectX::XMVectorDivide(a, b), m2), DirectX::XMVectorZero(), DirectX::XMVectorReplicate(1.0f));
	return ldr;
}	