#pragma once
#include "BaseWindow.h"
#include <vector>
#include <functional>

class MainWindow : public BaseWindow
{
public:
	MainWindow() = default;
	virtual ~MainWindow();

	virtual HWND Create(int x, int y, RECT wr, LPCTSTR pszTitle, DWORD dwStyle, DWORD dwStyleEx, HMENU pszMenu, HINSTANCE hInstance, HWND hwndParent) override;

	void Flush();

	inline std::vector<int32_t>& GetPixels(){ return m_Pixels; }
	
	void BindEventCallback(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> f) { m_EventCallback = f; }
protected:
	virtual LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual void OnClassCreation(WNDCLASSEX& wcex) override;
	virtual LPCTSTR ClassName() override;
private:
	HDC m_HDC;
	BITMAPINFO m_BMI;
	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> m_EventCallback = nullptr;
	std::vector<int32_t> m_Pixels;
};

extern const int RESOLUTION_DECREASE_COEF;