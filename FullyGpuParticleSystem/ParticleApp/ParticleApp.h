#pragma once

#include "Core/MainWindow.h"
#include "Util/MathHelper.h"
#include "Model/PassConstants.h"

#include "../Resource.h"

#include <unordered_map>

class ParticleSystem;
class ParticleSystemManager;
class PassConstantBuffer;
class TextureBuffer;
class ImguiInitializer;
class NodeEditorEmit;
class ParticleSystemController;
struct MeshGeometry;
struct Material;

struct ID3D12GraphicsCommandList;

class ShaderStatementNodeCurlNoiseForce;

struct ParticleAppVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

class ParticleApp : public MainWindow
{
public:
	ParticleApp(HINSTANCE hInstance);
	virtual ~ParticleApp();

	virtual PCWSTR getClassName() const override;

	virtual bool initialize() override;

	void onMouseLeftDown(int x, int y, short keyState) override;
	void onMouseLeftUp(int x, int y, short keyState) override;
	void onMouseMove(int x, int y, short keyState) override;

private:
	virtual void onResize() override;
	virtual void update(const GameTimer& gt) override;
	virtual void draw(const GameTimer& gt) override;

	void drawObjects(const GameTimer& gt);
	void drawUi();

	void loadTextures(ID3D12GraphicsCommandList* commandList);
	void registerCbvSrvUavDescriptors();
	void buildMaterials();
	void buildRootSignature();
	void buildShadersAndInputLayout();
	void buildPso();

	void updateCamera(const GameTimer& gt);

	void fireDrawToParticleSystems(ID3D12GraphicsCommandList* cmdList, const GameTimer& gt);

private:
	ComPtr<ID3D12RootSignature> _rootSignature = nullptr;

	ComPtr<ID3DBlob> _vsByteCode = nullptr;
	ComPtr<ID3DBlob> _psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

	std::shared_ptr<PassConstantBuffer> _passConstantBuffer;
	std::unique_ptr<MeshGeometry> _boxGeometry = nullptr;
	std::unordered_map<std::string, std::unique_ptr<TextureBuffer>> _textures;
	std::unordered_map<std::string, std::unique_ptr<Material>> _materials;

	ComPtr<ID3D12PipelineState> _pso = nullptr;

	PassConstants _passConstants;

	DirectX::XMFLOAT3 _eyePos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 _view = MathHelper::identity4x4();
	DirectX::XMFLOAT4X4 _proj = MathHelper::identity4x4();

	float theta = 1.5f * DirectX::XM_PI;
	float phi = DirectX::XM_PIDIV4;
	float radius = 5.0f;

	POINT lastMousePos;

	std::unique_ptr<ParticleSystemManager> _particleSystemManager;
	std::unique_ptr<ImguiInitializer> _imguiInitializer;
	std::unique_ptr<ParticleSystemController> _particleSystemController;
	std::unique_ptr<ShaderStatementNodeCurlNoiseForce> _temp;
};
