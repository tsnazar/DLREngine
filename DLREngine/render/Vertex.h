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
		PosTex,
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

	struct VertexPosTex
	{
		VertexPosTex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texCoord) : pos(position), texCoord(texCoord) {}

		DirectX::XMFLOAT3 pos = { 0 , 0 , 0 };
		DirectX::XMFLOAT2 texCoord = { 0 , 0 };
	};

	template<typename T>
	VertexType GetVertexTypeFromStruct()
	{
		if (std::is_same<T, VertexPos>::value)
			return VertexType::Pos;
		else if (std::is_same<T, VertexPosCol>::value)
			return VertexType::PosCol;
		else if (std::is_same<T, VertexPosTex>::value)
			return VertexType::PosTex;
		else
			return VertexType::Undefined;
	}
}