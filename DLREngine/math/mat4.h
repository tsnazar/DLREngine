#pragma once
#include "vec4.h"

namespace math
{
	struct mat4
	{
		vec4 cols[4];

		mat4(){}
		mat4(float s) : cols{ vec4(s,0,0,0), vec4(0,s,0,0), vec4(0,0,s,0), vec4(0,0,0,s)} {}
		mat4(float x0, float y0, float z0, float w0,
			 float x1, float y1, float z1, float w1,
			 float x2, float y2, float z2, float w2,
			 float x3, float y3, float z3, float w3) 
			: cols{vec4(x0, y0, z0, w0), vec4(x1, y1, z1, w1) , vec4(x2, y2, z2, w2) , vec4(x3, y3, z3, w3) } {}
		
		mat4(const vec4& c0, const vec4& c1, const vec4& c2, const vec4& c3)
			: cols{c0, c1, c2, c3} {}
		mat4(const mat4& m)
		{
			this->cols[0] = m.cols[0];
			this->cols[1] = m.cols[1];
			this->cols[2] = m.cols[2];
			this->cols[3] = m.cols[3];
		}

		inline vec4& operator[](int i)
		{
			return cols[i];
		}

		inline const mat4& operator+() const { return *this; }
		inline mat4 operator-() const { return mat4(-this->cols[0], -this->cols[1], -this->cols[2], -this->cols[3]); }

		inline mat4& operator+=(const mat4& rhs)
		{
			this->cols[0] += rhs.cols[0];
			this->cols[1] += rhs.cols[1];
			this->cols[2] += rhs.cols[2];
			this->cols[3] += rhs.cols[3];
			return *this;
		}
		inline mat4& operator-=(const mat4& rhs)
		{
			this->cols[0] -= rhs.cols[0];
			this->cols[1] -= rhs.cols[1];
			this->cols[2] -= rhs.cols[2];
			this->cols[3] -= rhs.cols[3];
			return *this;
		}
		inline mat4& operator*=(const mat4& rhs)
		{
			this->cols[0] *= rhs.cols[0];
			this->cols[1] *= rhs.cols[1];
			this->cols[2] *= rhs.cols[2];
			this->cols[3] *= rhs.cols[3];
			return *this;
		}
		inline mat4& operator/=(const mat4& rhs)
		{
			this->cols[0] /= rhs.cols[0];
			this->cols[1] /= rhs.cols[1];
			this->cols[2] /= rhs.cols[2];
			this->cols[3] /= rhs.cols[3];
			return *this;
		}

		friend inline mat4 operator+(const mat4& lhs, const mat4& rhs)
		{
			return mat4(lhs.cols[0] + rhs.cols[0], lhs.cols[1] + rhs.cols[1], lhs.cols[2] + rhs.cols[2], lhs.cols[3] + rhs.cols[3]);
		}
		friend inline mat4 operator-(const mat4& lhs, const mat4& rhs)
		{
			return mat4(lhs.cols[0] - rhs.cols[0], lhs.cols[1] - rhs.cols[1], lhs.cols[2] - rhs.cols[2], lhs.cols[3] - rhs.cols[3]);
		}
		friend inline mat4 operator*(const mat4& lhs, const mat4& rhs)
		{
			return mat4(lhs.cols[0] * rhs.cols[0], lhs.cols[1] * rhs.cols[1], lhs.cols[2] * rhs.cols[2], lhs.cols[3] * rhs.cols[3]);
		}

		friend inline mat4 operator*(float lhs, const mat4& rhs)
		{
			return mat4(lhs * rhs.cols[0], lhs * rhs.cols[1], lhs * rhs.cols[2], lhs * rhs.cols[3]);
		}

		friend inline vec4 operator*(const vec4& lhs, const mat4& rhs)
		{
			return vec4(dot(lhs, rhs.cols[0]), dot(lhs, rhs.cols[1]), dot(lhs, rhs.cols[2]), dot(lhs, rhs.cols[3]));
		}


		friend inline mat4 operator*(const mat4& lhs, float rhs)
		{
			return mat4(lhs.cols[0] * rhs, lhs.cols[1] * rhs, lhs.cols[2] * rhs, lhs.cols[3] * rhs);
		}
		friend inline mat4 operator/(const mat4& lhs, float rhs)
		{
			return mat4(lhs.cols[0] / rhs, lhs.cols[1] / rhs, lhs.cols[2] / rhs, lhs.cols[3] * rhs);
		}

		friend bool operator==(const mat4& lhs, const mat4& rhs)
		{
			return (lhs.cols[0] == rhs.cols[0] && lhs.cols[1] == rhs.cols[1] && lhs.cols[2] == rhs.cols[2] && lhs.cols[3] == lhs.cols[3]);
		}

		friend bool operator!=(const mat4& lhs, const mat4& rhs)
		{
			return !(lhs == rhs);
		}
	};

	inline mat4 ortho(float l, float r, float b, float t, float n, float f)
	{
		mat4 mat(0.0f);
		mat[0][0] =  2.0f / (r - l);
		mat[1][1] =  2.0f / (t - b);
		mat[2][2] = -2.0f / (f - n);
		mat[0][3] = -(r + l) / (r - l);
		mat[1][3] = -(t + b) / (t - b);
		mat[2][3] = -(f + n) / (f - n);
		mat[3][3] = 1.0f;
		return mat;
	}
}
