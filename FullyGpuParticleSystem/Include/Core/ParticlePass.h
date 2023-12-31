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
class ShaderStatementGraph;

class ParticlePass : public Hashable
{
public:
	ParticlePass(ParticleResource* resource, std::string name);
	virtual ~ParticlePass();

	std::string getName();

	ID3D12RootSignature* getRootSignature();

	void setShaderStatementGraph(std::shared_ptr<ShaderStatementGraph> graph);
	std::shared_ptr<ShaderStatementGraph> getShaderStatementGraph();

protected:
	bool isShaderDirty() const;
	void setShaderDirty(bool newIsShaderDirty);
	virtual void onShaderStatementGraphChanged() {};

	virtual std::vector<CD3DX12_ROOT_PARAMETER> buildRootParameter() = 0;
	virtual int getNumSrvUsing() = 0;
	virtual int getNumUavUsing() = 0;
	virtual bool needsStaticSampler() = 0;

	void buildRootSignature();

	Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;

	ParticleResource* _resource;

	std::shared_ptr<ShaderStatementGraph> _shaderStatementGraph;

private:
	bool _isShaderDirty;

	std::string _name;
};