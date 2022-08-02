#pragma once
#include "Texture2D.h"

namespace engine
{
	struct Material
	{
		Texture2D* texture;

		bool operator<(const Material& other)
		{
			return texture < texture;
		}

		friend bool operator==(const Material& lhs, const Material& rhs)
		{
			return lhs.texture == rhs.texture;
		}
	};
}

namespace std
{
	template<>
	struct hash<engine::Material>
	{
		std::size_t operator()(const engine::Material& k) const
		{
			hash<engine::Texture2D*> h;
			return h(k.texture);
		}
	};
}