#pragma once
#include <math.h>
#include <iostream>

namespace math
{
	struct vec4
	{
		float x, y, z, w;

		vec4() : x(0), y(0), z(0), w(0) {};
		vec4(float e0, float e1, float e2, float e3) : x(e0), y(e1), z(e2), w(e3) {};

		inline float& operator[](int i)
		{
			return *(&x + i);
		}

		inline const vec4& operator+() const { return *this; }
		inline vec4 operator-() const { return vec4(-x, -y, -z, -w); }

		inline vec4& operator+=(const vec4& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}
		inline vec4& operator-=(const vec4& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}
		inline vec4& operator*=(const vec4& rhs)
		{
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			w *= rhs.w;
			return *this;
		}
		inline vec4& operator/=(const vec4& rhs)
		{
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			w /= rhs.w;
			return *this;
		}

		inline vec4& operator/=(const float rhs)
		{
			x /= rhs;
			y /= rhs;
			z /= rhs;
			w /= rhs;
			return *this;
		}
		inline vec4& operator*=(const float rhs)
		{
			x *= rhs;
			y *= rhs;
			z *= rhs;
			w *= rhs;
			return *this;
		}

		friend inline std::istream& operator>>(std::istream& is, vec4& t)
		{
			is >> t.x >> t.y >> t.z >> t.w;
			return is;
		}

		friend inline std::ostream& operator<<(std::ostream& os, const vec4& t)
		{
			os << t.x << " " << t.y << " " << t.z << " " << t.w;
			return os;
		}

		friend inline vec4 operator+(const vec4& lhs, const vec4& rhs)
		{
			return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
		}
		friend inline vec4 operator-(const vec4& lhs, const vec4& rhs)
		{
			return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
		}
		friend inline vec4 operator*(const vec4& lhs, const vec4& rhs)
		{
			return vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
		}
		friend inline vec4 operator/(const vec4& lhs, const vec4& rhs)
		{
			return vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
		}

		friend inline vec4 operator*(float lhs, const vec4& rhs)
		{
			return vec4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
		}


		friend inline vec4 operator*(const vec4& lhs, float rhs)
		{
			return vec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
		}
		friend inline vec4 operator/(const vec4& lhs, float rhs)
		{
			return vec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w * rhs);
		}

		friend bool operator==(const vec4& lhs, const vec4& rhs)
		{
			return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == lhs.w);
		}

		friend bool operator!=(const vec4& lhs, const vec4& rhs)
		{
			return !(lhs == rhs);
		}
	};

	inline float dot(const vec4& lhs, const vec4& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
	}

	inline float length(const vec4& vec)
	{
		return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w + vec.w);
	}

	inline vec4 normalize(vec4 vec)
	{
		return vec / length(vec);
	}
}