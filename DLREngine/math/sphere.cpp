#include "sphere.h"

bool math::sphere::hit(const ray& casted_ray, float t_min, float t_max, hit_record& record) const
{
	vec3 oc = casted_ray.origin - center;
	float a = dot(casted_ray.direction, casted_ray.direction);
	float b = dot(casted_ray.direction, oc);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0)
	{
		float sqrtDiscriminant = sqrt(discriminant);
		float temp = (-b > sqrtDiscriminant) ? (-b - sqrtDiscriminant) / a : (-b + sqrtDiscriminant) / a;
		if (temp < t_max && temp > t_min)
		{
			record.t = temp;
			record.point = casted_ray.point_at_line(temp);
			record.normal = (record.point - center) / radius;
			return true;
		}
	}
	return false;
}