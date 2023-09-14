#pragma once
#include <vector>
#include "MathUtils.h"
#include <algorithm>
#include "windows/winapi.hpp"

namespace engine
{

	struct Point
	{
		DirectX::XMFLOAT2 pos;
		bool active = false;
	};

	struct Grid
	{
		Grid(int width, int height, float radius)
			: width(width), height(height), cellSize(radius/sqrtf(2)), nCellsWidth(ceilf(width / cellSize)), nCellsHeight(ceilf(height / cellSize))
		{
			points.resize(nCellsWidth * nCellsHeight);
		}

		void insertPoint(const Point& p)
		{
			int xIndex = floor(p.pos.x / cellSize);
			int yIndex = floor(p.pos.y / cellSize);
			points[nCellsWidth * yIndex + xIndex] = p;
		}

		bool isValidPoint(Point p, float radius)
		{
			if (p.pos.x < 0 || p.pos.x >= width || p.pos.y < 0 || p.pos.y >= height)
				return false;

			int xIndex = floor(p.pos.x / cellSize);
			int yIndex = floor(p.pos.y / cellSize);
			int i0 = std::max(xIndex - 1, 0);
			int i1 = std::min(xIndex + 1, nCellsWidth - 1);
			int j0 = std::max(yIndex - 1, 0);
			int j1 = std::min(yIndex + 1, nCellsHeight - 1);

			for (int i = i0; i <= i1; ++i)
			{
				for (int j = j0; j <= j1; ++j)
				{
					Point p0 = points[nCellsWidth * j + i];
					if (p0.active != false)
						if (sqrtf((p0.pos.x - p.pos.x) * (p0.pos.x - p.pos.x) + (p0.pos.y - p.pos.y) * (p0.pos.y - p.pos.y)) <  radius)
							return false;
				}
			}

			return true;
		}

		int width;
		int height;
		float cellSize;
		int nCellsWidth;
		int nCellsHeight;

		std::vector<Point> points;
	};


	void poissonDiskSampling(int width, int height, float radius, int k, std::vector<DirectX::XMFLOAT2>& points);
}