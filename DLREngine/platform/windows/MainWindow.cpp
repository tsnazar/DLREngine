#include "MainWindow.h"

MainWindow::~MainWindow()
{
	if (m_HDC)
		ReleaseDC(m_HandleWnd, m_HDC);
}

HWND MainWindow::Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent)
{
	BaseWindow::Create(x, y, wr, pszTitle, dwStyle, dwStyleEx, pszMenu, hInstance, hwndParent);
	m_HDC = GetDC(m_HandleWnd);
	return m_HandleWnd;
}

LRESULT MainWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
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
