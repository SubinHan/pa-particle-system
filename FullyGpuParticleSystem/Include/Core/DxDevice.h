#pragma once

#include <Windows.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <d3d12.h>
#include <windows.foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <DirectXColors.h>

using Microsoft::WRL::ComPtr;

class ICbvSrvUavDemander;

class DxDevice
{
public:
	DxDevice(HWND mainWindow);

	void createDepthStencilView();
	void updateScreenViewport();
	void updateScissorRect();

	ComPtr<ID3D12GraphicsCommandList> startRecordingCommands(void);
	void submitCommands(ComPtr<ID3D12GraphicsCommandList> commandList);

	void registerCbvSrvUavDescriptorDemander(
		ICbvSrvUavDemander* demander
	);
	// build descriptor heap sized by registered descriptor demanders.
	void buildCbvSrvUavDescriptorHeap();

	void ResetCommandList();
	ID3D12Resource* getCurrentBackBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE getCurrentBackBufferViewHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE getDepthStencilViewHandle();
	ComPtr<ID3D12DescriptorHeap> GetCbvSrvUavDescriptorHeap();
	void flushCommandQueue();
	void resetAllSwapChainBuffers();
	void resetDepthStencilBuffer();
	void resizeBuffers();

	ComPtr<ID3D12Device> getD3dDevice();
	ComPtr<ID3D12CommandQueue> getCommandQueue();
	ComPtr<ID3D12CommandAllocator> getCommandListAllocator();
	ComPtr<ID3D12GraphicsCommandList> getCommandList();
	ComPtr<IDXGISwapChain> getSwapChain();
	void swapBuffers();

	DXGI_FORMAT getBackBufferFormat();
	DXGI_FORMAT getDepthStencilFormat();
	bool getMsaaState();
	UINT getMsaaQuality();
	void enable4xMsaa();

	UINT getClientWidth();
	UINT getClientHeight();
	void setClientWidth(UINT width);
	void setClientHeight(UINT height);
	UINT getSwapChainBufferCount();

	ComPtr<ID3D12Fence> getFence();
	D3D12_VIEWPORT& getScreenViewport();
	tagRECT& getScissorRect();
	ID3D12Resource* getDepthStencilBuffer();
	UINT getCbvSrvUavDescriptorSize();
	UINT getDsvDescriptorSize();
	UINT getRtvDescriptorSize();

	ComPtr<ID3D12DescriptorHeap> getDsvHeap();
	ComPtr<ID3D12DescriptorHeap> getRtvHeap();

	UINT getCurrentFence();
	UINT increaseFence();


public:
	static constexpr UINT SWAP_CHAIN_BUFFER_COUNT = 2;

	ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	ComPtr<ID3D12DescriptorHeap> _cbvSrvUavHeap;

private:
	void init();
	void createDevice();
	void createFence();
	void getDescriptorHandleIncrementSize();
	void checkMsaa();
	void createCommandQueueAndList();
	void createSwapChain();
	void createRtvAndDsvDescriptorHeaps();
	void createRenderTargetView();

private:
	ComPtr<IDXGIFactory>	_dxgiFactory;
	ComPtr<ID3D12Device>	_d3dDevice;
	ComPtr<ID3D12Fence>		_fence;

	UINT _rtvDescriptorSize = 0;
	UINT _dsvDescriptorSize = 0;
	UINT _cbvSrvUavDescriptorSize = 0;

	ComPtr<ID3D12CommandQueue>			_commandQueue;
	ComPtr<ID3D12CommandAllocator>		_commandListAllocator;
	ComPtr<ID3D12GraphicsCommandList>	_commandList;
	ComPtr<IDXGISwapChain>				_swapChain;
	ComPtr<ID3D12Resource>				_swapChainBuffers[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID3D12Resource>				_depthStencilBuffer;

	UINT		_msaaQuality;
	bool		_msaaState;
	DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	HWND _mainWindow;
	UINT _clientWidth;
	UINT _clientHeight;
	UINT _clientRefreshRate;

	UINT _currentBackBufferIndex = 0;
	UINT _currentFence = 0;

	D3D12_VIEWPORT	_screenViewport;
	tagRECT			_scissorRect;

	std::vector<ICbvSrvUavDemander*> _cbvSrvUavDescriptorDemanders;
};