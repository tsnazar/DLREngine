#include "MainWindow.h"

#include <DirectXMath.h>

#include "Debug.h"
#include "ApplicationEvent.h"
#include "KeyEvents.h"
#include "MouseEvents.h"

namespace engine
{
	MainWindow::~MainWindow()
	{
		if (m_HDC)
			ReleaseDC(m_HandleWnd, m_HDC);
	}

	HWND MainWindow::Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent)
	{
		BaseWindow::Create(x, y, wr, pszTitle, dwStyle, dwStyleEx, pszMenu, hInstance, hwndParent);
		m_HDC = GetDC(m_HandleWnd);

		InitSwapchain();

		return m_HandleWnd;
	}

	void MainWindow::InitSwapchain()
	{
		// Create Swapchain
		DXGI_SWAP_CHAIN_DESC1 desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC1));

		desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = 0;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Scaling = DXGI_SCALING_NONE;
		desc.Stereo = false;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		HRESULT result = engine::s_Factory->CreateSwapChainForHwnd(engine::s_Device, m_HandleWnd, &desc, NULL, NULL, m_Swapchain.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));
	}

	void MainWindow::InitBackbuffer()
	{
		uint32_t width = m_ClientWidth != 0 ? m_ClientWidth : 8;
		uint32_t height = m_ClientHeight != 0 ? m_ClientHeight : 8;

		m_Backbuffer.Release();
		m_Swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		
		/*if (m_Backbuffer.valid())
		{
			m_Backbuffer.release();
		}*/

		// Create Backbuffer
		//ID3D11Texture2D* pTextureInterface = nullptr;
		//HRESULT result = m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pTextureInterface);
		//ALWAYS_ASSERT(SUCCEEDED(result));

		//result = s_Device->CreateRenderTargetView(pTextureInterface, NULL, m_Backbuffer.reset());
		//ALWAYS_ASSERT(SUCCEEDED(result));
		//pTextureInterface->Release();

		m_Backbuffer.GetBackbufferFromSwapchain(m_Swapchain.ptr());

		// Create Viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = m_ClientWidth;
		viewport.Height = m_ClientHeight;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		s_Devcon->RSSetViewports(1, &viewport);
	}

	void MainWindow::ClearColor(const float color[4])
	{
		s_Devcon->ClearRenderTargetView(m_Backbuffer.GetRenderTarget(), color);
	}

	void MainWindow::Flush()
	{
		m_Swapchain->Present(0, 0);
	}

	//return true if window alive, false otherwies
	bool MainWindow::PoolEvents()
	{
		while (PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&m_Msg);
			DispatchMessage(&m_Msg);

			if (m_Msg.message == WM_QUIT)
			{
				return false;
			}
		}
		return true;
	}

	LRESULT MainWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			WindowCloseEvent e; 
			if (m_EventCallback != nullptr)
				m_EventCallback(e);
			return 0;
		}
		case WM_SIZE:
		{
			RECT rt;

			GetWindowRect(m_HandleWnd, &rt);
			m_Width = rt.right - rt.left;
			m_Height = rt.bottom - rt.top;

			GetClientRect(m_HandleWnd, &rt);
			m_ClientWidth = rt.right - rt.left;
			m_ClientHeight = rt.bottom - rt.top;

			InitBackbuffer();
			WindowResizeEvent e(m_ClientWidth, m_ClientHeight);
			if(m_EventCallback != nullptr)
				m_EventCallback(e);

			return 0;
		}
		case WM_KEYDOWN:
		{
			m_Keys[wParam] = true;
			KeyPressedEvent e(static_cast<KeyCode>(wParam));
			m_EventCallback(e);
			return 0;
		}
		case WM_KEYUP:
		{
			m_Keys[wParam] = false;
			KeyReleasedEvent e(static_cast<KeyCode>(wParam));
			m_EventCallback(e);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			m_Keys[Key::LMB] = true;
			MouseButtonPressedEvent e(Key::LMB);
			m_EventCallback(e);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			m_Keys[Key::RMB] = true;
			MouseButtonPressedEvent e(Key::RMB);
			m_EventCallback(e);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			m_Keys[Key::LMB] = false;
			MouseButtonReleasedEvent e(Key::LMB);
			m_EventCallback(e);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			m_Keys[Key::RMB] = false;
			MouseButtonReleasedEvent e(Key::RMB);
			m_EventCallback(e);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			MouseScrolledEvent e(0, GET_WHEEL_DELTA_WPARAM(wParam));
			m_EventCallback(e);
			return 0;
		}
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void MainWindow::OnClassCreation(WNDCLASSEX& wcex)
	{
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	}

	LPCTSTR MainWindow::ClassName()
	{
		return TEXT("MainWindow");
	}

}