#pragma once
#include <Windows.h>
#include <vector>
#include <memory>

#include "windows/MainWindow.h"
#include "windows/winapi.hpp"
#include "geometry/Mesh.h"
#include "geometry/Plane.h"
#include "geometry/Sphere.h"
#include "lights/DirectionLight.h"
#include "lights/PointLight.h"
#include "lights/SpotLight.h"
#include "math/IObjectMover.h"
#include "Camera.h"
#include "Transform.h"

class Scene
{
protected:

	enum class IntersectedType { Sphere, Plane, Transform, Light, NUM };

	struct ObjRef
	{
		void* object;
		IntersectedType type;
	};
public:

	struct Sphere : public math::Sphere
	{
		math::Material material;

		Sphere() {}
		Sphere(DirectX::XMFLOAT3 pos, float radius, math::Material material) : math::Sphere(pos, radius), material(material) {}
		bool Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial);
	};

	struct Plane : public math::Plane
	{
		math::Material material;

		Plane() {}
		Plane(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 normal, const math::Material& material) : math::Plane(pos, normal), material(material) {}
		bool Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& record, const math::Material*& outMaterial);
	};

	struct Transform : public math::Transform
	{
		math::Material material;
		math::Mesh& mesh;

		Transform(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, const math::Material& material, math::Mesh& mesh)
			: math::Transform(scale, rotation, position), material(material), mesh(mesh) {}
		bool Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& record, const math::Material*& outMaterial);
	};

	struct DirectionLight : public math::DirectionLight
	{
		math::Material material;

		DirectionLight() {}

		DirectionLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 intensity) :math::DirectionLight(direction, intensity), material(intensity, intensity, 0.0f, 0.0f) {}

		bool Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& record, const math::Material*& outMaterial);

		DirectX::XMVECTOR Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
			const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material);
	};

	struct PointLight : public math::PointLight
	{
		math::Material material;

		PointLight() {}

		PointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 intensity, float radius) :math::PointLight(position, intensity, radius), material(intensity, intensity, 0.0f, 0.0f) {}

		bool Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& record, const math::Material*& outMaterial);

		DirectX::XMVECTOR Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
			const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material);
	};

	struct SpotLight : public math::SpotLight
	{
		math::Material material;

		SpotLight() {}

		SpotLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 intensity, float radius, float innerRad, float outerRad)
			: math::SpotLight(position, direction, intensity, radius, innerRad, outerRad), material(intensity, intensity, 0.0f, 0.0f) {}

		bool Intersect(const math::Ray& ray, ObjRef& outRef, math::Intersection& record, const math::Material*& outMaterial);

		DirectX::XMVECTOR Illuminate(Scene& scene, const DirectX::XMVECTOR& toCameraDir, const DirectX::XMVECTOR& pixelPos,
			const DirectX::XMVECTOR& pixelNormal, const math::MaterialVectorized& material);
	};

	struct IntersectionQuery
	{
		math::Intersection intersection;
		const math::Material* material;

		std::unique_ptr<IObjectMover>* mover;
	};

public:
	Scene();

	bool Render(MainWindow& win, Camera& camera);

	void AddSphereToScene(const DirectX::XMFLOAT3& pos, float radius, const math::Material& material)
		{ m_Spheres.emplace_back(pos, radius, material); }

	void AddPlaneToScene(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& normal, const math::Material& material)
		{ m_Planes.emplace_back(pos, normal, material); }
	
	void AddCubeToScene(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, const math::Material& material) 
		{ m_CubeTransforms.emplace_back(scale, rotation, position, material, m_CubeMesh);}

	void AddDirLightToScene(const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& intensity) 
		{ m_DirLights.emplace_back(dir, intensity); }

	void AddPointLightToScene(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& intensity, float lightRadius)
		{ m_PointLights.emplace_back(pos, intensity, lightRadius); }

	void AddSpotLightToScene(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& intensity, float lightRadius, float innerRad, float outerRad) 
		{ m_SpotLights.emplace_back(pos, dir, intensity, lightRadius, innerRad, outerRad); }

	bool FindIntersection(const math::Ray& ray, math::Intersection& outNearest, const math::Material*& outMaterial, bool onlyObjects = false);

	bool FindIntersection(const math::Ray& ray, IntersectionQuery& query, bool onlyObjects = false);

protected:

	void FindIntersectionInternal(const math::Ray& ray, ObjRef& outRef, math::Intersection& outNearest, const math::Material*& outMaterial, bool onlyObjects);

	DirectX::XMFLOAT3 ComputeColor(const math::Ray& castedRay, const DirectX::XMVECTOR& cameraPos);



private:
	std::vector<Sphere> m_Spheres;
	std::vector<Plane> m_Planes;
	std::vector<DirectionLight> m_DirLights;
	std::vector<PointLight> m_PointLights;
	std::vector<SpotLight> m_SpotLights;
	std::vector<Transform> m_CubeTransforms;
	math::Mesh m_CubeMesh = math::Mesh::createCube();
};

DirectX::XMVECTOR NdcToWorld(DirectX::XMVECTOR pos, const DirectX::XMMATRIX& mat);
