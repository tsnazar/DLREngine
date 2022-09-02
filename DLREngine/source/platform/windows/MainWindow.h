#pragma once
#include "BaseWindow.h"
#include "Globals.h"
#include <vector>
#include <functional>
#include "KeyCodes.h"
#include "Event.h"
#include "RenderTarget.h"

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

		void InitSwapchain();

		void InitBackbuffer();

		void ClearColor(const float color[4]);

		void Flush();

		bool PoolEvents();

		void BindEventCallback(std::function<void(Event&)> f) { m_EventCallback = f; }

		bool IsKeyPressed(KeyCode keycode) const { return m_Keys[keycode]; }

		//DxResPtr<ID3D11RenderTargetView>& GetRenderTarget() { return m_Backbuffer; }
		// 
		RenderTarget& GetBackBuffer() { return m_Backbuffer; }

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
		//DxResPtr<ID3D11RenderTargetView> m_Backbuffer;
		RenderTarget m_Backbuffer;
	};
}

