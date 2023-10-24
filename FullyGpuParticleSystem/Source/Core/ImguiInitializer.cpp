#include "Core/ImguiInitializer.h"

#include "Core/DxDevice.h"

#include "imnodes.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

ImguiInitializer::ImguiInitializer(DxDevice* device, HWND hwnd) :
	_device(device),
	_hwnd(hwnd)
{
}

int ImguiInitializer::getNumDescriptorsToDemand() const
{
	return 1;
}

void ImguiInitializer::buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu)
{
	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImNodes::CreateContext();
	ImNodes::EditorContextCreate();

	ImGuiIO& io = ImGui::GetIO();
	ImNodesIO& nodesIo = ImNodes::GetIO();

	nodesIo.LinkDetachWithModifierClick.Modifier = &io.KeyCtrl;
	nodesIo.MultipleSelectModifier.Modifier = &io.KeyCtrl;

	ImNodesStyle& style = ImNodes::GetStyle();
	style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(_hwnd);
	ImGui_ImplDX12_Init(
		_device->getD3dDevice().Get(),
		1,
		_device->getBackBufferFormat(),
		_device->getCbvSrvUavDescriptorHeap().Get(),
		hCpu,
		hGpu);
}
