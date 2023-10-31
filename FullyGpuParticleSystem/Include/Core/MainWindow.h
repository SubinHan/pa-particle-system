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

	BOOL create(
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

	int run();
	static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	float aspectRatio() const;
	virtual bool initialize();

	virtual PCWSTR getClassName() const = 0;
	virtual LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void createDevice();
	void releaseDevice();

	virtual void onResize();
	virtual void update(const GameTimer& gt) = 0;
	virtual void draw(const GameTimer& gt) = 0;

	virtual void onMouseLeftDown(int x, int y, short keyState) {};
	virtual void onMouseLeftUp(int x, int y, short keyState) {};
	virtual void onMouseMiddleDown(int x, int y, short keyState) {};
	virtual void onMouseMiddleUp(int x, int y, short keyState) {};
	virtual void onMouseRightDown(int x, int y, short keyState) {};
	virtual void onMouseRightUp(int x, int y, short keyState) {};
	virtual void onMouseXDown(int x, int y, short keyState) {};
	virtual void onMouseXUp(int x, int y, short keyState) {};
	virtual void onMouseWheel(short delta, short keyState) {}
	virtual void onMouseHover(int x, int y) {}
	virtual void onMouseLeave() {}
	virtual void onMouseMove(int x, int y, short keyState) {}

	virtual void onKeyDown(WPARAM windowVirtualKeyCode) {}
	virtual void onKeyUp(WPARAM windowVirtualKeyCode) {}


	void calculateFrameStats();

	bool _isPaused = false;
	bool _isMinimized = false;
	bool _isMaximized = false;
	bool _isResizing = false;
	bool _isFullscreenActivated = false;

protected:
	GameTimer _timer;
	std::wstring _mainWndCaption;

	HWND _hwnd;
	HINSTANCE _hInstance;
	DxDevice* _device;
};