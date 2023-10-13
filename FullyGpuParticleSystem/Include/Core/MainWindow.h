#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <windows.foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>

#include <memory>
#include "Core/DxDevice.h"
#include "Util/DxDebug.h"
#include "Util/GameTimer.h"

class MainWindow
{
public:
	MainWindow(HINSTANCE hInstance);
	virtual ~MainWindow();

	BOOL Create(
		PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hWndParent = 0,
		HMENU hMenu = 0
	);

	int Run();
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	float aspectRatio() const;
	virtual bool initialize();

	virtual PCWSTR getClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void CreateDevice();
	void ReleaseDevice();

	virtual void OnResize();
	virtual void update(const GameTimer& gt) = 0;
	virtual void draw(const GameTimer& gt) = 0;

	virtual void OnMouseLeftDown(int x, int y, short keyState) {};
	virtual void OnMouseLeftUp(int x, int y, short keyState) {};
	virtual void OnMouseMiddleDown(int x, int y, short keyState) {};
	virtual void OnMouseMiddleUp(int x, int y, short keyState) {};
	virtual void OnMouseRightDown(int x, int y, short keyState) {};
	virtual void OnMouseRightUp(int x, int y, short keyState) {};
	virtual void OnMouseXDown(int x, int y, short keyState) {};
	virtual void OnMouseXUp(int x, int y, short keyState) {};
	virtual void OnMouseWheel(short delta, short keyState) {}
	virtual void OnMouseHover(int x, int y) {}
	virtual void OnMouseLeave() {}
	virtual void OnMouseMove(int x, int y, short keyState) {}

	virtual void OnKeyDown(WPARAM windowVirtualKeyCode) {}
	virtual void OnKeyUp(WPARAM windowVirtualKeyCode) {}


	void CalculateFrameStats();

	bool isPaused = false;
	bool isMinimized = false;
	bool isMaximized = false;
	bool isResizing = false;
	bool isFullscreenActivated = false;

protected:
	GameTimer timer;
	std::wstring mainWndCaption;

	HWND _hwnd;
	HINSTANCE _hInstance;
	std::unique_ptr<DxDevice> _device;
};