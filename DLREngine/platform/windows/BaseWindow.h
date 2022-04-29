#pragma once
#include <Windows.h>
#include <Windowsx.h>

struct WindowProps
{
	int x;
	int y;
	RECT wr;
	LPCTSTR pszTitle;
	DWORD dwStyle;
	DWORD dwStyleEx;
	HMENU pszMenu;
	HINSTANCE hInstance;
	HWND hwndParent;
};

class BaseWindow
{
public:
	BaseWindow() = default;
	virtual ~BaseWindow();

	BaseWindow(const BaseWindow&) = delete;
	BaseWindow& operator=(const BaseWindow&) = delete;
	BaseWindow(BaseWindow&&) = delete;
	BaseWindow& operator=(BaseWindow&&) = delete;

	virtual HWND Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent);

	bool Show(int nShowCmd);

	inline HWND GetWindow() const { return m_HandleWnd; }

	inline int GetWidth() const { return m_Width; }

	inline int GetHeight() const { return m_Height; }	

	inline int GetClientWidth() const { return m_ClientWidth; }

	inline int GetClientHeight() const { return m_ClientHeight; }

protected:
	static LRESULT CALLBACK s_WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	virtual LPCTSTR ClassName() = 0;

	virtual LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	virtual void OnClassCreation(WNDCLASSEX& wcex) = 0;

protected:
	int m_Width, m_Height;
	int m_ClientWidth, m_ClientHeight;
	HWND m_HandleWnd;
};

