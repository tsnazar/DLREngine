#include "MathUtils.h"
#include <algorithm>
#include "windows/winapi.hpp"

using namespace DirectX;

namespace engine
{
	void LoadMatrixInArray(const XMMATRIX& matrix, XMFLOAT4 arr[4])
	{
		XMStoreFloat4(&arr[0], matrix.r[0]);
		XMStoreFloat4(&arr[1], matrix.r[1]);
		XMStoreFloat4(&arr[2], matrix.r[2]);
		XMStoreFloat4(&arr[3], matrix.r[3]);
	}

	XMVECTOR findMaxComponent(const XMVECTOR& vec)
	{
		return XMVectorReplicate((std::max)((std::max)(XMVectorGetX(vec), XMVectorGetY(vec)), XMVectorGetZ(vec)));
	}
}