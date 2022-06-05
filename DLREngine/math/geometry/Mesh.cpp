#include "Mesh.h"

bool math::Mesh::Intersect(const Ray& ray, Intersection& record) const
{
	bool hit = false;
	for (const auto& trig : triangles)
		hit |= trig.Intersect(ray, record);
	return hit;	
}
