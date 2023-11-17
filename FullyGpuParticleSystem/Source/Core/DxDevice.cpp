#include "Core/DxDevice.h"
#include "Util/DxDebug.h"
#include "Core/ICbvSrvUavDemander.h"

void DxDevice::init()
{
    createDevice();
    createFence();
    getDescriptorHandleIncrementSize();
    checkMsaa();
    createCommandQueueAndList();
    createSwapChain();
    createRtvAndDsvDescriptorHeaps();
    createRenderTargetView();
}

void DxDevice::createDevice()
{
    UINT dxgiFactoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG) 
    // Enable the D3D12 debug layer.
    {
        //ComPtr<ID3D12Debug> debugController;
        //ComPtr<ID3D12Debug3> debugController1;
        //ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
        //ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings)));
        //ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        //debugController->QueryInterface(IID_PPV_ARGS(&debugController1));
        //debugController->EnableDebugLayer();
        //debugController1->SetEnableGPUBasedValidation(true);
        //dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        //dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

        //dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_dxgiFactory)));

    HRESULT hardwareResult = D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&_d3dDevice)
    );
}

void DxDevice::createFence()
{
    ThrowIfFailed(_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
}

void DxDevice::getDescriptorHandleIncrementSize()
{
    _rtvDescriptorSize = _d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _dsvDescriptorSize = _d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    _cbvSrvUavDescriptorSize = _d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void DxDevice::checkMsaa()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels = {};
    msQualityLevels.Format = _backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));

    _msaaQuality = msQualityLevels.NumQualityLevels;
    assert(_msaaQuality > 0 && "Unexpected MSAA quality level.");
}

void DxDevice::createCommandQueueAndList()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)));
    ThrowIfFailed(_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_commandListAllocator.GetAddressOf())));
    ThrowIfFailed(_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandListAllocator.Get(), nullptr, IID_PPV_ARGS(_commandList.GetAddressOf())));
    _commandList->Close();
}

void DxDevice::createSwapChain()
{
    _swapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = _clientWidth;
    sd.BufferDesc.Height = _clientHeight;
    sd.BufferDesc.RefreshRate.Numerator = _clientRefreshRate;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = _backBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = _msaaState ? 4 : 1;
    sd.SampleDesc.Quality = _msaaState ? (_msaaQuality - 1) : 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
    sd.OutputWindow = _mainWindow;
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ThrowIfFailed(_dxgiFactory->CreateSwapChain(_commandQueue.Get(), &sd, _swapChain.GetAddressOf()));
}

void DxDevice::createRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.GetAddressOf())));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 2;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.GetAddressOf())));
}

void DxDevice::createRenderTargetView()
{
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(&_swapChainBuffers[i])));
        _d3dDevice->CreateRenderTargetView(_swapChainBuffers[i].Get(), nullptr, rtvHeapHandle);

        // rtvHeapHandle.Offset(1, rtvDescriptorSize) of a type CD3DX12_CPU_DESCRIPTOR_HANDLE
        rtvHeapHandle.ptr = SIZE_T(INT64(rtvHeapHandle.ptr) + INT64(1) * INT64(_rtvDescriptorSize));
    }
}

DxDevice& DxDevice::getInstance()
{
    static DxDevice instance;

    return instance;
}

void DxDevice::initDevice(HWND mainWindow)
{
    auto& device = getInstance();

    device._mainWindow = mainWindow;

    RECT windowRect;
    if (GetWindowRect(mainWindow, &windowRect))
    {
        device._clientWidth = windowRect.right - windowRect.left;
        device._clientHeight = windowRect.bottom - windowRect.top;
    }
    device._clientRefreshRate = 165;

    device.init();

    assert(device._d3dDevice);
    assert(device._swapChain);
    assert(device._commandListAllocator);

    device._isInitiated = true;
}

void DxDevice::createDepthStencilView()
{
    D3D12_HEAP_PROPERTIES heapProperties;
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = _clientWidth;
    depthStencilDesc.Height = _clientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = _depthStencilFormat;
    depthStencilDesc.SampleDesc.Count = _msaaState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = _msaaState ? (_msaaQuality - 1) : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear = {};
    optClear.Format = _depthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    ThrowIfFailed(_d3dDevice->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(_depthStencilBuffer.GetAddressOf())
    ));

    _d3dDevice->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _depthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE
    );

    _commandList->ResourceBarrier(
        1,
        &barrier
    );
}

void DxDevice::updateScreenViewport()
{
    _screenViewport.TopLeftX = 0.0f;
    _screenViewport.TopLeftY = 0.0f;
    _screenViewport.Width = static_cast<float>(_clientWidth);
    _screenViewport.Height = static_cast<float>(_clientHeight);
    _screenViewport.MinDepth = 0.0f;
    _screenViewport.MaxDepth = 1.0f;
}

void DxDevice::updateScissorRect()
{
    _scissorRect = { 
        0, 
        0,
        static_cast<long>(_clientWidth), 
        static_cast<long>(_clientHeight) };
}

ComPtr<ID3D12GraphicsCommandList> DxDevice::startRecordingCommands(void)
{
    ++_recordingCommands;
    if (_recordingCommands > 1)
    {
        return _commandList;
    }

    ThrowIfFailed(_commandListAllocator->Reset());
    _commandList->Reset(_commandListAllocator.Get(), nullptr);

    return _commandList;
}

void DxDevice::submitCommands(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    --_recordingCommands;
    if (_recordingCommands != 0)
        return;

    ThrowIfFailed(commandList->Close());

    ID3D12CommandList* cmdsLists[] = { commandList.Get() };
    _commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    flushCommandQueue();

    if (_needsUpdateDescriptorHeap)
    {
        buildCbvSrvUavDescriptorHeap();
        _needsUpdateDescriptorHeap = false;
    }
}

void DxDevice::registerCbvSrvUavDescriptorDemander(ICbvSrvUavDemander* demander)
{
    _cbvSrvUavDescriptorDemanders.push_back(demander);

    _needsUpdateDescriptorHeap = true;
}

void DxDevice::registerCbvSrvUavDescriptorDemander(std::shared_ptr<ICbvSrvUavDemander> demander)
{
    _cbvSrvUavDescriptorDemandersShared.push_back(demander);

    _needsUpdateDescriptorHeap = true;
}

void DxDevice::unregisterCbvSrvUavDescriptorDemander(ICbvSrvUavDemander* demander)
{
    for (int i = 0; i < _cbvSrvUavDescriptorDemanders.size(); ++i)
    {
        if (_cbvSrvUavDescriptorDemanders[i] == demander)
        {
            _cbvSrvUavDescriptorDemanders.erase(_cbvSrvUavDescriptorDemanders.begin() + i);
            break;
        }
    }

    for (int i = 0; i < _cbvSrvUavDescriptorDemandersShared.size(); ++i)
    {
        if (_cbvSrvUavDescriptorDemandersShared[i].get() == demander)
        {
            _cbvSrvUavDescriptorDemandersShared.erase(_cbvSrvUavDescriptorDemandersShared.begin() + i);
            break;
        }
    }

    return;
}

void DxDevice::buildCbvSrvUavDescriptorHeap()
{
    int numDescriptors = 0;

    for (auto& demander : _cbvSrvUavDescriptorDemanders)
    {
        numDescriptors += demander->getNumDescriptorsToDemand();
    }

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&_cbvSrvUavHeap)));

    // Initialize descreiptors

    auto cpuHandle =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(_cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());
    auto gpuHandle =
        CD3DX12_GPU_DESCRIPTOR_HANDLE(_cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());

    for (auto& demander : _cbvSrvUavDescriptorDemanders)
    {
        const auto numDescriptorsToDemand = demander->getNumDescriptorsToDemand();

        demander->buildCbvSrvUav(cpuHandle, gpuHandle);
        cpuHandle.Offset(numDescriptorsToDemand, _cbvSrvUavDescriptorSize);
        gpuHandle.Offset(numDescriptorsToDemand, _cbvSrvUavDescriptorSize);
    }

    for (auto& demander : _cbvSrvUavDescriptorDemandersShared)
    {
        const auto numDescriptorsToDemand = demander->getNumDescriptorsToDemand();

        demander->buildCbvSrvUav(cpuHandle, gpuHandle);
        cpuHandle.Offset(numDescriptorsToDemand, _cbvSrvUavDescriptorSize);
        gpuHandle.Offset(numDescriptorsToDemand, _cbvSrvUavDescriptorSize);
    }
}

void DxDevice::ResetCommandList()
{
    ThrowIfFailed(_commandListAllocator->Reset());
    ThrowIfFailed(_commandList->Reset(_commandListAllocator.Get(), nullptr));

    auto currentBackBuffer = getCurrentBackBuffer();
    auto currentBackBufferView = getCurrentBackBufferViewHandle();
    auto depthStencilView = getDepthStencilViewHandle();

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer,
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    _commandList->ResourceBarrier(1, &barrier);

    _commandList->RSSetViewports(1, &_screenViewport);
    _commandList->RSSetScissorRects(1, &_scissorRect);

    _commandList->ClearRenderTargetView(
        currentBackBufferView,
        DirectX::Colors::Black, 0, nullptr);
    _commandList->ClearDepthStencilView(
        getDepthStencilViewHandle(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.0f,
        0,
        0,
        nullptr);

    _commandList->OMSetRenderTargets(1, &currentBackBufferView,
        true, &depthStencilView);

    auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    _commandList->ResourceBarrier(
        1,
        &barrier2
    );

    ThrowIfFailed(_commandList->Close());
    ID3D12CommandList* commandLists[] = { _commandList.Get() };
    _commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    ThrowIfFailed(_swapChain->Present(0, 0));
    _currentBackBufferIndex = (_currentBackBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;

    flushCommandQueue();
}

ID3D12Resource* DxDevice::getCurrentBackBuffer()
{
    return _swapChainBuffers[_currentBackBufferIndex].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DxDevice::getCurrentBackBufferViewHandle()
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        _rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        _currentBackBufferIndex,
        _rtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE DxDevice::getDepthStencilViewHandle()
{
    return _dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

ComPtr<ID3D12DescriptorHeap> DxDevice::getCbvSrvUavDescriptorHeap()
{
    return _cbvSrvUavHeap;
}

void DxDevice::flushCommandQueue()
{
    increaseFence();

    ThrowIfFailed(_commandQueue->Signal(_fence.Get(), _currentFence));

    // wait until all tasks complete
    if (_fence->GetCompletedValue() < _currentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr,
            FALSE, false, EVENT_ALL_ACCESS);
        ThrowIfFailed(_fence->SetEventOnCompletion(_currentFence, eventHandle));

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void DxDevice::resetAllSwapChainBuffers()
{
    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        _swapChainBuffers[i].Reset();
    }
}

void DxDevice::resetDepthStencilBuffer()
{
    _depthStencilBuffer.Reset();
}

void DxDevice::resizeBuffers()
{
    ThrowIfFailed(_swapChain->ResizeBuffers(
        SWAP_CHAIN_BUFFER_COUNT,
        _clientWidth,
        _clientHeight,
        _backBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    ));

    _currentBackBufferIndex = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
    {
        ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(&_swapChainBuffers[i])));
        _d3dDevice->CreateRenderTargetView(_swapChainBuffers[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, _rtvDescriptorSize);
    }
}


ComPtr<ID3D12Device> DxDevice::getD3dDevice()
{
    return _d3dDevice;
}

ComPtr<ID3D12CommandQueue> DxDevice::getCommandQueue()
{
    return _commandQueue;
}

ComPtr<ID3D12CommandAllocator> DxDevice::getCommandListAllocator()
{
    return _commandListAllocator;
}

ComPtr<ID3D12GraphicsCommandList> DxDevice::getCommandList()
{
    return _commandList;
}

void DxDevice::swapBuffers()
{
    ThrowIfFailed(_swapChain->Present(0, 0));
    _currentBackBufferIndex = (_currentBackBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

UINT DxDevice::getCurrentFence()
{
    return _currentFence;
}

UINT DxDevice::increaseFence()
{
    return ++_currentFence;
}

void DxDevice::handleDeviceLost()
{
#if defined(DEBUG) || defined(_DEBUG) 
    ComPtr<ID3D12DeviceRemovedExtendedData> dred;
    ThrowIfFailed(_d3dDevice->QueryInterface(IID_PPV_ARGS(&dred)));
    D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT DredAutoBreadcrumbsOutput;
    D3D12_DRED_PAGE_FAULT_OUTPUT DredPageFaultOutput;
    ThrowIfFailed(dred->GetAutoBreadcrumbsOutput(&DredAutoBreadcrumbsOutput));
    ThrowIfFailed(dred->GetPageFaultAllocationOutput(&DredPageFaultOutput));
    HRESULT removedReason = _d3dDevice->GetDeviceRemovedReason();
    _com_error err(removedReason);
    OutputDebugString(err.ErrorMessage());
#endif
}

UINT DxDevice::getClientWidth()
{
    return _clientWidth;
}

UINT DxDevice::getClientHeight()
{
    return _clientHeight;
}

void DxDevice::setClientWidth(UINT width)
{
    _clientWidth = width;
}

void DxDevice::setClientHeight(UINT height)
{
    _clientHeight = height;
}

ComPtr<ID3D12Fence> DxDevice::getFence()
{
    return _fence;
}

D3D12_VIEWPORT& DxDevice::getScreenViewport()
{
    return _screenViewport;
}

tagRECT& DxDevice::getScissorRect()
{
    return _scissorRect;
}

DXGI_FORMAT DxDevice::getBackBufferFormat()
{
    return _backBufferFormat;
}

DXGI_FORMAT DxDevice::getDepthStencilFormat()
{
    return _depthStencilFormat;
}

bool DxDevice::getMsaaState()
{
    return _msaaState;
}

UINT DxDevice::getMsaaQuality()
{
    return _msaaQuality;
}
