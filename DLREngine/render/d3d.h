#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <d3d10effect.h>


namespace engine
{
	extern ID3D11Device5* s_Device;
	extern ID3D11DeviceContext4* s_Devcon;
	extern IDXGIFactory5* s_Factory;
	extern ID3D11Debug* s_Debug;
}