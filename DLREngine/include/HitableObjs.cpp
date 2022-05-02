#include "HitableObjs.h"

void HitableObjs::push_back(std::unique_ptr<math::hitable>&& el)
{
	m_Objects.push_back(std::move(el));
}

bool HitableObjs::hit(const math::ray& casted_ray, float t_min, float t_max, math::hit_record& rec) const
{
	math::hit_record temp;
	bool hitted = false;
	double closest_so_far = t_max;
	for (const auto& h : m_Objects)
	{
		if (h->hit(casted_ray, t_min, t_max, temp))
		{
			hitted = true;
			closest_so_far = temp.t;
			rec = temp;
		}
	}
	return hitted;
}