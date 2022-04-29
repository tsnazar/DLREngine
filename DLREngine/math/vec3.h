#pragma once
#include <math.h>
#include <iostream>

namespace math
{
	struct vec3
	{
		float x, y, z;

		vec3() : x(0), y(0), z(0) {};
		vec3(float e0, float e1, float e2) : x(e0), y(e1), z(e2) {};

		inline float& operator[](int i)
		{
			return *(&x + i);
		}

		inline const vec3& operator+() const { return *this; }
		inline vec3 operator-() const { return vec3(-x, -y, -z); }

		inline vec3& operator+=(const vec3& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		inline vec3& operator-=(const vec3& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}
		inline vec3& operator*=(const vec3& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}
		inline vec3& operator/=(const vec3& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}

		inline vec3& operator/=(const float rhs)
		{
			x /= rhs;
			y /= rhs;
			z /= rhs;
			return *this;
		}
		inline vec3& operator*=(const float rhs)
		{
			x *= rhs;
			y *= rhs;
			z *= rhs;
			return *this;
		}

		friend inline std::istream& operator>>(std::istream& is, vec3& t)
		{
			is >> t.x >> t.y >> t.z;
			return is;
		}

		friend inline std::ostream& operator<<(std::ostream& os, const vec3& t)
		{
			os << t.x << " " << t.y << " " << t.z;
			return os;
		}

		friend inline vec3 operator+(const vec3& lhs, const vec3& rhs)
		{
			return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
		}
		friend inline vec3 operator-(const vec3& lhs, const vec3& rhs)
		{
			return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
		}
		friend inline vec3 operator*(const vec3& lhs, const vec3& rhs)
		{
			return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
		}
		friend inline vec3 operator/(const vec3& lhs, const vec3& rhs)
		{
			return vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
		}

		friend inline vec3 operator*(float lhs, const vec3& rhs)
		{
			return vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
		}


		friend inline vec3 operator*(const vec3& lhs, float rhs)
		{
			return vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
		}
		friend inline vec3 operator/(const vec3& lhs, float rhs)
		{
			return vec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
		}

		friend bool operator==(const vec3& lhs, const vec3& rhs)
		{
			return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z);
		}

		friend bool operator!=(const vec3& lhs, const vec3& rhs)
		{
			return !(lhs == rhs);
		}
	};

	inline float dot(const vec3& lhs, const vec3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline vec3 cross(const vec3& lhs, const vec3& rhs)
	{
		return vec3((lhs.y * rhs.z - lhs.z * rhs.y), 
				   -(lhs.x * rhs.z - lhs.z * rhs.x), 
					(lhs.x * rhs.y - lhs.y * rhs.x));
	}

	inline float length(const vec3& vec)
	{
		return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	inline vec3 normalize(vec3 vec)
	{
		return vec / length(vec);
	}
}