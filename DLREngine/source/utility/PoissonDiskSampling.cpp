#include "PoissonDiskSampling.h"

namespace engine
{
	void poissonDiskSampling(int width, int height, float radius, int k, std::vector<DirectX::XMFLOAT2>& points)
	{
		Grid grid(width, height, radius);

		std::vector<Point> activePoints;

		Point p0 = { {rand() / static_cast<float>(RAND_MAX) / width, rand() / static_cast<float>(RAND_MAX) / height}, true };

		grid.insertPoint(p0);
		activePoints.push_back(p0);
		points.push_back(p0.pos);

		while (activePoints.size() > 0)
		{
			int index = rand() % activePoints.size();

			Point p = activePoints[index];

			bool found = false;

			for (int tries = 0; tries < k; ++tries)
			{
				float theta = rand() % 360 / 180.0f * DirectX::XM_PI;
				float newRadius = radius + (rand() / (RAND_MAX / radius));
				float pNewX = p.pos.x + newRadius * cosf(theta);
				float pNewY = p.pos.y + newRadius * sinf(theta);
				Point pNew = { {pNewX, pNewY}, true };

				if (!grid.isValidPoint(pNew, radius))
					continue;

				activePoints.push_back(pNew);
				grid.insertPoint(pNew);
				points.push_back(pNew.pos);
				found = true;
				break;
			}

			if (!found)
				activePoints.erase(activePoints.begin() + index);
		}
	}
}