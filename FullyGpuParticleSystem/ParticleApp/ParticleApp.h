#pragma once

#include "Core/MainWindow.h"
#include "Model/Geometry.h"
#include "Util/MathHelper.h"
#include "ParticleResource.h"
#include "ParticleEmitter.h"

#include "../Resource.h"

class ObjectConstantBuffer;

struct ParticleAppVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

class ParticleApp : public MainWindow
{
public:
	ParticleApp(HINSTANCE hInstance);

	virtual PCWSTR getClassName() const override;

	virtual bool initialize() override;

	void onMouseLeftDown(int x, int y, short keyState) override;
	void onMouseLeftUp(int x, int y, short keyState) override;
	void onMouseMove(int x, int y, short keyState) override;

private:
	virtual void OnResize() override;
	virtual void update(const GameTimer& gt) override;
	virtual void draw(const GameTimer& gt) override;

	void buildCbvSrvUavDescriptors();
	void buildRootSignature();
	void buildShadersAndInputLayout();
	void buildBoxGeometry();
	void buildPso();

private:
	ComPtr<ID3D12RootSignature> _rootSignature = nullptr;

	ComPtr<ID3DBlob> _vsByteCode = nullptr;
	ComPtr<ID3DBlob> _psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

	std::shared_ptr<ObjectConstantBuffer> _objectConstantBuffer;
	std::unique_ptr<MeshGeometry> _boxGeometry = nullptr;

	ComPtr<ID3D12PipelineState> _pso = nullptr;

	DirectX::XMFLOAT4X4 _matrixProjection = MathHelper::identity4x4();

	float theta = 1.5f * DirectX::XM_PI;
	float phi = DirectX::XM_PIDIV4;
	float radius = 5.0f;

	POINT lastMousePos;

	std::unique_ptr<ParticleResource> _particleResource;
	std::unique_ptr<ParticleEmitter> _particleEmitter;
};
