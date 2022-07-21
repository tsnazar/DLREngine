#pragma once
#include "BaseWindow.h"
#include "Globals.h"
#include <vector>
#include <functional>
#include "KeyCodes.h"
#include "Event.h"

namespace engine
{
	namespace
	{
		const short NUM = 256;
	}

	class MainWindow : public BaseWindow
	{
	public:
		MainWindow() = default;
		virtual ~MainWindow();

		virtual HWND Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent) override;

		void InitSwapchainAndStates();

		void InitDepthAndBackbuffer();

		void ClearColor(const float color[4]);

		void BindRenderTarget();

		void Flush();

		bool PoolEvents();

		void BindEventCallback(std::function<void(Event&)> f) { m_EventCallback = f; }

		bool IsKeyPressed(KeyCode keycode) const { return m_Keys[keycode]; }

	protected:
		virtual LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

		virtual void OnClassCreation(WNDCLASSEX& wcex) override;

		virtual LPCTSTR ClassName() override;
	
	private:
		HDC m_HDC;
		MSG m_Msg;
		bool m_Keys[NUM];

		std::function<void(Event&)> m_EventCallback = nullptr;

		DxResPtr<IDXGISwapChain1> m_Swapchain;
		DxResPtr<ID3D11RenderTargetView> m_Backbuffer;
		DxResPtr<ID3D11DepthStencilView> m_Depthbuffer;
		DxResPtr<ID3D11DepthStencilState> m_DepthState;
	};
}

