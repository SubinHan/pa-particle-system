#ifndef UNICODE
#define UNICODE
#endif 

#include "ParticleApp/ParticleApp.h"

#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    try {
        ParticleApp win(hInstance);

        if (!win.create(L"Learn to Program Windows", WS_OVERLAPPEDWINDOW))
        {
            return 0;
        }

        win.run();
    }
    catch (DxException& e)
    {
        OutputDebugString(L"Exception:");
        OutputDebugString(e.toString().c_str());
        MessageBox(nullptr, e.toString().c_str(), L"HR Failed", MB_OK);
    }

    return 0;
}