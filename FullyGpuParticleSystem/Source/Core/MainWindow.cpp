#include "Core/MainWindow.h"

#include <WindowsX.h>
#include <memory>

MainWindow::MainWindow(HINSTANCE hInstance) :
	_hwnd(NULL), 
	_hInstance(hInstance)
{
}

MainWindow::~MainWindow()
{
	CoUninitialize();
}

BOOL MainWindow::Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
    WNDCLASS wc = { 0 };

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
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

int MainWindow::Run()
{
    ShowWindow(_hwnd, SW_SHOWNORMAL);

    // Run the message loop.
    bool bGotMsg;
    MSG  msg;
    timer.reset();
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


        timer.tick();

        if (isPaused)
        {
            Sleep(100);
            continue;
        }

        ////CalculateFrameStats();
        update(timer);
        draw(timer);
    }

    return 0;
}

LRESULT MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        pThis->HandleMessage(uMsg, wParam, lParam);
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
    CreateDevice();

    return true;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

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
            isPaused = true;
            timer.stop();
            return 0;
        }

        isPaused = false;
        timer.start();
        return 0;

    case WM_SIZE:
        // Save the new client area dimensions.
        _device->setClientWidth(LOWORD(lParam));
        _device->setClientHeight(HIWORD(lParam));
        if (_device->getD3dDevice())
        {
            if (wParam == SIZE_MINIMIZED)
            {
                isPaused = true;
                isMinimized = true;
                isMaximized = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                isPaused = false;
                isMinimized = false;
                isMaximized = true;
                onResize();
            }
            else if (wParam == SIZE_RESTORED)
            {

                // Restoring from minimized state?
                if (isMinimized)
                {
                    isPaused = false;
                    isMinimized = false;
                    onResize();
                }

                // Restoring from maximized state?
                else if (isMaximized)
                {
                    isPaused = false;
                    isMaximized = false;
                    onResize();
                }
                else if (isResizing)
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
        isPaused = true;
        isResizing = true;
        timer.stop();
        return 0;

    case WM_EXITSIZEMOVE:
        isPaused = false;
        isResizing = false;
        timer.start();
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
        OnMouseMiddleDown(xPos, yPos, keyState);
        break;
    }
    case WM_MBUTTONUP:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseMiddleUp(xPos, yPos, keyState);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseRightDown(xPos, yPos, keyState);
        break;
    }
    case WM_RBUTTONUP:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseRightUp(xPos, yPos, keyState);
        break;
    }
    case WM_XBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseXDown(xPos, yPos, keyState);
        break;
    }
    case WM_XBUTTONUP:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseXUp(xPos, yPos, keyState);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), keyState);
        break;
    }
    case WM_MOUSEHOVER:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseHover(xPos, yPos);
        break;
    }
    case WM_MOUSELEAVE:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        int keyState = GET_KEYSTATE_WPARAM(wParam);
        OnMouseLeave();
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
        OnKeyDown(wParam);
        break;
    }
    case WM_KEYUP:
    {
        OnKeyUp(wParam);
    }

    default:
        return DefWindowProc(_hwnd, uMsg, wParam, lParam);
    }

    return TRUE;
}

void MainWindow::CreateDevice()
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
