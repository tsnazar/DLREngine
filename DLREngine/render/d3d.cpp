#include "d3d.h"

namespace engine
{
	ID3D11Device5* s_Device = nullptr;
	ID3D11DeviceContext4* s_Devcon = nullptr;
	IDXGIFactory5* s_Factory = nullptr;
}