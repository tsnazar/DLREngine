#pragma once

#include <DirectXMath.h>
#include <type_traits>

namespace engine
{
	enum class VertexType
	{
		Undefined,
		Pos,
		PosCol,
	};

	struct VertexUndefined
	{

	};

	struct VertexPos
	{
		VertexPos(const DirectX::XMFLOAT3& position) : pos(position) {}

		DirectX::XMFLOAT3 pos = { 0 , 0 , 0 };
	};

	struct VertexPosCol
	{
		VertexPosCol(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color) : pos(position), col(color) {}

		DirectX::XMFLOAT3 pos = { 0 , 0 , 0 };
		DirectX::XMFLOAT4 col = { 0 , 0 , 0 , 0 };
	};


}