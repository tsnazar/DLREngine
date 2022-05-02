#pragma once
#include <vector>

#include "hitable.h"
#include <memory>

class HitableObjs : public math::hitable
{
public:
	HitableObjs() {}
	HitableObjs(size_t capacity) { m_Objects.reserve(capacity); }

	void push_back(std::unique_ptr<math::hitable>&& el);

	virtual bool hit(const math::ray& casted_ray, float t_min, float t_max, math::hit_record& rec) const;
private:
	std::vector<std::unique_ptr<math::hitable>> m_Objects;
};

