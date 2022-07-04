#pragma once
#include "BaseWindow.h"
#include "Globals.h"
#include <vector>
#include <functional>

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

	void BindEventCallback(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> f) { m_EventCallback = f; }

protected:
	virtual LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	
	virtual void OnClassCreation(WNDCLASSEX& wcex) override;
	
	virtual LPCTSTR ClassName() override;
private:
	HDC m_HDC;
	
	//BITMAPINFO m_BMI;

	//int m_ImageWidth, m_ImageHeight;
	
	//unsigned int m_ResolutionDecreaseCoef = 2.0f;
	
	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> m_EventCallback = nullptr;
	
	engine::DxResPtr<IDXGISwapChain1> m_Swapchain;
	engine::DxResPtr<ID3D11RenderTargetView> m_Backbuffer;
	D3D11_TEXTURE2D_DESC m_BackbufferDesc;

	//std::vector<int32_t> m_Pixels;
};

