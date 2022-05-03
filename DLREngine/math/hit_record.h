#pragma once
#include "vec3.h"

namespace math
{
	struct hit_record
	{
		float t;
		vec3 point;
		vec3 normal;
	};
}