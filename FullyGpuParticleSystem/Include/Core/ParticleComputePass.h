#pragma once

#include "Core/ParticlePass.h"

class ParticleComputePass : public ParticlePass
{
public:
	ParticleComputePass(ParticleResource* resource, std::string name);

	void setComputeShader(Microsoft::WRL::ComPtr<ID3DBlob> shader);

protected:
	virtual std::vector<CD3DX12_ROOT_PARAMETER>  buildRootParameter() override;
	virtual int getNumSrvUsing() override;
	virtual int getNumUavUsing() override;
	virtual bool needsStaticSampler() override;
	virtual int getNum32BitsConstantsUsing() = 0;

	// particles root parameter uses slot 1~5
	void setParticlesComputeRootUav(ID3D12GraphicsCommandList* commandList);
	void bindComputeResourcesOfRegisteredNodes(ID3D12GraphicsCommandList* commandList, int startRootSlot);

	// set 32bit constants. it dependes on getNum32BitsConstantsUsing().
	void setConstants(ID3D12GraphicsCommandList* commandList, const void* constants);

	// ready to dispatch except setting 32bit constants.
	// you should invoke setConstants() before dispatch.
	void readyDispatch(ID3D12GraphicsCommandList* commandList);

protected:
	ID3DBlob* getComputeShader() const;

private:
	void rebuildComputePsos();

private:
	Microsoft::WRL::ComPtr<ID3DBlob> _computeShader;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> _pso;

	CD3DX12_DESCRIPTOR_RANGE _counterUavTable;
	CD3DX12_DESCRIPTOR_RANGE _passCbvTable;
};