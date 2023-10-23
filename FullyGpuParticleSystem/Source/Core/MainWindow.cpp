#include "Core/MainWindow.h"

#include <WindowsX.h>
#include <memory>

#include "imgui_impl_win32.h"

MainWindow::MainWindow(HINSTANCE hInstance) :
	_hwnd(NULL), 
	_hInstance(hInstance)
{
}

MainWindow::~MainWindow()
{
	CoUninitialize();
}

BOOL MainWindow::create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
    WNDCLASS wc = { 0 };

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = windowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = getClassName();

    RegisterClass(&wc);

    _hwnd = CreateWindowEx(
        dwExStyle, getClassName(), lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    return (_hwnd ? TRUE : FALSE);
}

int MainWindow::run()
{
    ShowWindow(_hwnd, SW_SHOWNORMAL);

    // Run the message loop.
    bool bGotMsg;
    MSG  msg;
    _timer.reset();
    msg.message = WM_NULL;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

    while (WM_QUIT != msg.message)
    {
        // Process window events.
        // Use PeekMessage() so we can use idle time to render the scene. 
        bGotMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

        if (bGotMsg)
        {
            // Translate and dispatch the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }


        _timer.tick();

        if (_isPaused)
        {
            Sleep(100);
            continue;
        }

        calculateFrameStats();
        update(_timer);
        draw(_timer);
    }

    return 0;
}

LRESULT MainWindow::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<MainWindow*>(create->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

        pThis->_hwnd = hwnd;
    }
    else
    {
        pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        pThis->handleMessage(uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

float MainWindow::aspectRatio() const
{

    return static_cast<float>(_device->getClientWidth()) /
        static_cast<float>(_device->getClientHeight());
}

bool MainWindow::initialize()
{
    createDevice();

    return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT MainWindow::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(_hwnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_CREATE:
        if (!initialize())
            return -1;
        return 0;

    case WM_DESTROY:
        ////ReleaseDevice();
        PostQuitMessage(0);
        return 0;

    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            _isPaused = true;
            _timer.stop();
            return 0;
        }

        _isPaused = false;
        _timer.start();
        return 0;

    case WM_SIZE:
        // Save the new client area dimensions.
        _device->setClientWidth(LOWORD(lParam));
        _device->setClientHeight(HIWORD(lParam));
        if (_device->getD3dDevice())
        {
            if (wParam == SIZE_MINIMIZED)
            {
                _isPaused = true;
                _isMinimized = true;
                _isMaximized = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                _isPaused = false;
                _isMinimized = false;
                _isMaximized = true;
                onResize();
            }
            else if (wParam == SIZE_RESTORED)
            {

                // Restoring from minimized state?
                if (_isMinimized)
                {
                    _isPaused = false;
                    _isMinimized = false;
                    onResize();
                }

                // Restoring from maximized state?
                else if (_isMaximized)
                {
                    _isPaused = false;
                    _isMaximized = false;
                    onResize();
                }
                else if (_isResizing)
                {
                    // If user is dragging the resize bars, we do not resize 
                    // the buffers here because as the user continuously 
                    // drags the resize bars, a stream of WM_SIZE messages are
                    // sent to the window, and it would be pointless (and slow)
                    // to resize for each WM_SIZE message received from dragging
                    // the resize bars.  So instead, we reset after the user is 
                    // done resizing the window and releases the resize bars, which 
                    // sends a WM_EXITSIZEMOVE message.
                }
                else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                {
                    onResize();
                }
            }
        }
        return 0;

    case WM_ENTERSIZEMOVE:
        _isPaused = true;
        _isResizing = true;
        _timer.stop();
        return 0;

    case WM_EXITSIZEMOVE:
        _isPaused = false;
        _isResizing = false;
        _timer.start();
        onResize();
        return 0;

    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
        return 0;

    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseLeftDown(xPos, yPos, keyState);
        break;
    }
    case WM_LBUTTONUP:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseLeftUp(xPos, yPos, keyState);
        break;
    }
    case WM_MBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseMiddleDown(xPos, yPos, keyState);
        break;
    }
    case WM_MBUTTONUP:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseMiddleUp(xPos, yPos, keyState);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseRightDown(xPos, yPos, keyState);
        break;
    }
    case WM_RBUTTONUP:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseRightUp(xPos, yPos, keyState);
        break;
    }
    case WM_XBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseXDown(xPos, yPos, keyState);
        break;
    }
    case WM_XBUTTONUP:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseXUp(xPos, yPos, keyState);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), keyState);
        break;
    }
    case WM_MOUSEHOVER:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseHover(xPos, yPos);
        break;
    }
    case WM_MOUSELEAVE:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseLeave();
        break;
    }
    case WM_MOUSEMOVE:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        onMouseMove(xPos, yPos, keyState);
        break;
    }
    case WM_KEYDOWN:
    {
        onKeyDown(wParam);
        break;
    }
    case WM_KEYUP:
    {
        onKeyUp(wParam);
    }

    default:
        return DefWindowProc(_hwnd, uMsg, wParam, lParam);
    }

    return TRUE;
}

void MainWindow::createDevice()
{
    _device = std::make_unique<DxDevice>(_hwnd);
    onResize();
}

void MainWindow::onResize()
{
    _device->flushCommandQueue();
    auto commandList = _device->getCommandList();
    auto commandQueue = _device->getCommandQueue();
    auto commandListAllocator = _device->getCommandListAllocator();

    ThrowIfFailed(commandList->Reset(commandListAllocator.Get(), nullptr));

    _device->resetAllSwapChainBuffers();
    _device->resetDepthStencilBuffer();
    _device->resizeBuffers();
    _device->createDepthStencilView();

    ThrowIfFailed(commandList->Close());
    ID3D12CommandList* cmdsLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    _device->flushCommandQueue();

    _device->updateScreenViewport();
    _device->updateScissorRect();
}

void MainWindow::calculateFrameStats()
{
    static int frameCnt = 0;
    static float timeElapsed = 0.0f;

    frameCnt++;
    if (_timer.totalTime() - timeElapsed >= 1.0f)
    {
        float fps = (float)frameCnt;
        float mspf = 1000.0f / fps;

        std::wstring fpsStr = std::to_wstring(fps);
        std::wstring mspfStr = std::to_wstring(mspf);

        std::wstring windowText = _mainWndCaption + 
			L"  fps: " + fpsStr +
            L"  mspf: " + mspfStr;

        SetWindowText(_hwnd, windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0f;
    }

}
