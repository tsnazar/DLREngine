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
		PosTexNorTan,
	};

	struct VertexUndefined
	{

	};

	struct VertexPos
	{
		VertexPos() = default;
		VertexPos(const DirectX::XMFLOAT3& position) : pos(position) {}

		DirectX::XMFLOAT3 pos = { 0 , 0 , 0 };
	};

	struct VertexPosCol
	{
		VertexPosCol() = default;
		VertexPosCol(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color) : pos(position), col(color) {}

		DirectX::XMFLOAT3 pos = { 0 , 0 , 0 };
		DirectX::XMFLOAT4 col = { 0 , 0 , 0 , 0 };
	};

	struct VertexPosTex
	{
		VertexPosTex() = default;
		VertexPosTex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texCoord) : pos(position), texCoord(texCoord) {}

		DirectX::XMFLOAT3 pos = { 0 , 0 , 0 };
		DirectX::XMFLOAT2 texCoord = { 0 , 0 };
	};

	struct VertexPosTexNorTan
	{
		VertexPosTexNorTan() = default;
		VertexPosTexNorTan(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texCoord, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent) 
			: pos(position), texCoord(texCoord), nor(normal), tan(tangent) {}

		DirectX::XMFLOAT3 pos = { 0 , 0 , 0 };
		DirectX::XMFLOAT2 texCoord = { 0 , 0 };
		DirectX::XMFLOAT3 nor = { 0, 0, 0 };
		DirectX::XMFLOAT3 tan = { 0, 0, 0 };
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
		else if (std::is_same<T, VertexPosTexNorTan>::value)
			return VertexType::PosTexNorTan;
		else
			return VertexType::Undefined;
	}
}