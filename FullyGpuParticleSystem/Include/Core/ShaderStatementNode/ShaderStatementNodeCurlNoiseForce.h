#pragma once

#include "Core/ShaderStatementNode/ShaderStatementNode.h"

#include "Core/ICbvSrvUavDemander.h"
#include "Util/MathHelper.h"

#include <wrl.h>

struct ID3D12Resource;
struct ID3D12GraphicsCommandList;

class ShaderStatementNodeCurlNoiseForce : public ShaderStatementNode, public ICbvSrvUavDemander
{
public:
	ShaderStatementNodeCurlNoiseForce(std::string variableName, float amplitude, float frequency);
	virtual ~ShaderStatementNodeCurlNoiseForce();

	virtual int getNumDescriptorsToDemand() const override;
	virtual void buildCbvSrvUav(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpu, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpu) override;

	virtual std::string generateStatements() const override;

	virtual int getNumResourcesToBind() const override;
	virtual ResourceViewType getResourceViewType(int index) const override;
	virtual std::string getTypeInShader(int index) const override;
	virtual void onResourceBound(std::vector<std::string> variableNamesInShader) override;
	virtual bool isResourceViewCreated(int index) const override;
	virtual CD3DX12_GPU_DESCRIPTOR_HANDLE getResourceGpuHandle(int index) const override;

private:
	void buildResource();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> _noiseTextureResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> _uploadBufferResource;

	float _amplitude;
	float _frequency;
	std::string _noiseTextureVariableName;

	CD3DX12_CPU_DESCRIPTOR_HANDLE _hCpu;
	CD3DX12_GPU_DESCRIPTOR_HANDLE _hGpu;

	bool _srvCreated;
	DirectX::XMFLOAT3* _noiseData;
};