#include "BaseWindow.h"

BaseWindow::~BaseWindow()
{
	if (m_HandleWnd)
	{
		SetWindowLongPtr(m_HandleWnd, GWLP_USERDATA, 0);
		DestroyWindow(m_HandleWnd);
	}
}

bool BaseWindow::Show(int nShowCmd)
{
	return ShowWindow(m_HandleWnd, nShowCmd);
}

LRESULT BaseWindow::s_WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BaseWindow* pThis;
	if (message == WM_CREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<BaseWindow*>(lpcs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else {
		pThis = reinterpret_cast<BaseWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	
	if (pThis)
	{
		return pThis->WindowProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND BaseWindow::Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent)
{
	m_ClientWidth = wr.right - wr.left;
	m_ClientHeight = wr.bottom - wr.top;

	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	m_Width = wr.right - wr.left;
	m_Height = wr.bottom - wr.top;
	
	WNDCLASSEX wcex{ sizeof(WNDCLASSEX), 0 };

	if (!GetClassInfoEx(hInstance, ClassName(), &wcex)) {
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = s_WindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.lpszClassName = ClassName();
		OnClassCreation(wcex);
		RegisterClassEx(&wcex);
	}
	m_HandleWnd = CreateWindowEx(dwStyleEx, ClassName(), pszTitle, dwStyle, x, y, m_Width, m_Height, hwndParent, pszMenu, hInstance, this);
	return m_HandleWnd;
}

