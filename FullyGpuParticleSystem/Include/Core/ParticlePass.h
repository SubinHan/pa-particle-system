#pragma once

#include "Core/Hashable.h"
#include "Util/DxUtil.h"

#include <string>
#include <memory>
#include <wrl.h>

struct ID3D12Device;
struct ID3D12PipelineState;
struct ID3D12RootSignature;
struct ID3D12GraphicsCommandList;

class ParticleResource;
class ShaderStatementNode;

class ParticlePass : public Hashable
{
public:
	ParticlePass(ParticleResource* resource, std::string name);

	std::string getName();

	ID3D12RootSignature* getRootSignature();
	ID3DBlob* getShader();
	ID3D12PipelineState* getPipelineStateObject();

	void registerShaderStatementNode(std::shared_ptr<ShaderStatementNode> node);
	void clearRegisteredShaderStatementNodes();
	void setShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);

protected:
	void bindComputeResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot);
	void bindGraphicsResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot);

	virtual std::vector<CD3DX12_ROOT_PARAMETER> buildRootParameter() = 0;
	virtual int getNumSrvUsing() = 0;
	virtual int getNumUavUsing() = 0;
	virtual bool needsStaticSampler() = 0;

	void buildRootSignature();
	virtual void buildPsos() = 0;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;
	Microsoft::WRL::ComPtr<ID3DBlob> _shader;

	ParticleResource* _resource;

	std::vector<std::shared_ptr<ShaderStatementNode>> _registeredNodes;

private:
	std::string _name;
};