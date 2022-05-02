#pragma once
#include "BaseWindow.h"
#include <functional>

class MainWindow : public BaseWindow
{
public:
	MainWindow() = default;
	virtual ~MainWindow();

	virtual HWND Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent) override;

	inline HDC GetHDC() const { return m_HDC; }
protected:
	virtual LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual void OnClassCreation(WNDCLASSEX& wcex) override;
	virtual LPCTSTR ClassName() override;
private:
	HDC m_HDC;
};