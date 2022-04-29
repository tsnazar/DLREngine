#pragma once
#include "BaseWindow.h"
#include <functional>

class MainWindow : public BaseWindow
{
public:
	using InputCallbackFunction = std::function<bool(UINT, WPARAM, LPARAM)>;

	MainWindow() = default;
	virtual ~MainWindow();

	virtual HWND Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent);

	inline HDC GetHDC() const { return m_HDC; }
protected:
	virtual LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnClassCreation(WNDCLASSEX& wcex);
	virtual LPCTSTR ClassName();
private:
	HDC m_HDC;
};