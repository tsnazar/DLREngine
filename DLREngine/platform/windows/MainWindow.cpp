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

	m_ImageWidth = m_ClientWidth / m_ResolutionDecreaseCoef;
	m_ImageHeight = m_ClientHeight / m_ResolutionDecreaseCoef;

	m_BMI.bmiHeader.biSize = sizeof(m_BMI);
	m_BMI.bmiHeader.biWidth = m_ImageWidth;
	m_BMI.bmiHeader.biHeight = m_ImageHeight;
	m_BMI.bmiHeader.biPlanes = 1;
	m_BMI.bmiHeader.biBitCount = 32;
	m_BMI.bmiHeader.biCompression = BI_RGB;

	m_Pixels.resize(m_ImageWidth * m_ImageHeight);

	return m_HandleWnd;
}

void MainWindow::Flush()
{
	StretchDIBits(m_HDC,
		0,
		0,
		m_ClientWidth,
		m_ClientHeight,
		0,
		0,
		m_ImageWidth,
		m_ImageHeight,
		m_Pixels.data(),
		&m_BMI,
		DIB_RGB_COLORS,
		SRCCOPY);
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

		m_ImageWidth = m_ClientWidth / m_ResolutionDecreaseCoef;
		m_ImageHeight = m_ClientHeight / m_ResolutionDecreaseCoef;

		m_BMI.bmiHeader.biWidth = m_ImageWidth;
		m_BMI.bmiHeader.biHeight = m_ImageHeight;
		m_Pixels.resize(m_ImageWidth * m_ImageHeight);
	}
	}

	if (m_EventCallback)
		return m_EventCallback(hWnd, message, wParam, lParam);

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
