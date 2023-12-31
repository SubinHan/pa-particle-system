#pragma once

#include "Core/ParticlePass.h"

#include "Util/DxUtil.h"

#include <memory>
#include <string>

class PassConstantBuffer;
class ParticleResource;
struct MeshGeometry;
struct ObjectConstants;


class ParticleRenderPass : public ParticlePass
{
public:
	ParticleRenderPass(ParticleResource* resource, std::string name);
	virtual ~ParticleRenderPass();

	virtual void render(
		ID3D12GraphicsCommandList* cmdList,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb) = 0;

	void bindGraphicsResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot);

	void setOpaqueness(bool newIsOpaque);
	void setBoundingMode(bool newIsBounding);
	void setWireframe(bool newIsWireframe);
	bool isOpaque();
	bool isBounding();
	bool isWireframe();

	std::string getBoundingTextureName();
	void setBoundingTextureName(std::string textureName);

	void setVertexShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setHullShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setDomainShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setGeometryShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);
	void setPixelShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);

	virtual std::string getPixelShaderEntryName() const = 0;

protected:
	virtual std::vector<CD3DX12_ROOT_PARAMETER> buildRootParameter() override;
	virtual int getNumSrvUsing() override;
	virtual int getNumUavUsing() override;
	virtual bool needsStaticSampler() override;

	ID3D12RootSignature* getIndirectCommandComputeRootSignature() const;
	ID3D12CommandSignature* getCommandSignature() const;
	ID3DBlob* getVertexShader() const;
	ID3DBlob* getHullShader() const;
	ID3DBlob* getDomainShader() const;
	ID3DBlob* getGeometryShader() const;
	ID3DBlob* getPixelShader() const;

	virtual void updateGeometryShader(bool isBounding) = 0;

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& getInputLayout();

	void computeIndirectCommand(
		ID3D12GraphicsCommandList* cmdList, 
		ID3D12PipelineState* computePso,
		UINT numDispatchBlocks);
	void executeIndirectCommand(
		ID3D12GraphicsCommandList* cmdList, 
		ID3D12PipelineState* renderPso,
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology,
		const ObjectConstants& objectConstants,
		const PassConstantBuffer& passCb);

	MeshGeometry* getEmptyGeometry();

private:
	void buildComputeIndirectRootSignature();
	void buildCommandSignature();
	void generateEmptyGeometry();

private:
	// for compute indirect commands.
	Microsoft::WRL::ComPtr<ID3D12RootSignature> _computeIndirectRootSignature;
	Microsoft::WRL::ComPtr<ID3D12CommandSignature> _commandSignature;

	CD3DX12_DESCRIPTOR_RANGE _passCbvTable;
	CD3DX12_DESCRIPTOR_RANGE _counterTable;
	CD3DX12_DESCRIPTOR_RANGE _texSrvTable;

	Microsoft::WRL::ComPtr<ID3DBlob> _shaderVs0;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderHs0;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderDs0;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderGs0;
	Microsoft::WRL::ComPtr<ID3DBlob> _shaderPs0;

	std::vector<D3D12_INPUT_ELEMENT_DESC> _inputLayout;

	std::unique_ptr<MeshGeometry> _emptyGeometry = nullptr;

	bool _isOpaque;
	bool _isBounding;
	bool _isWireframe;

	std::string _boundingTextureName;
	std::vector<DirectX::XMFLOAT2> _boundingPolygon;

private:

};