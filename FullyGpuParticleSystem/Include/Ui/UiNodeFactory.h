#pragma once

#include "Ui/UiNode.h"

enum class NodeType;

class UiNodeFactory
{
public:
	static UiNode createNode(const int id, NodeType nodeType);
	static UiNode createEmpty(const int id);
	static UiNode createNewFloat(const int id);
	static UiNode createNewFloat3(const int id);
	static UiNode createNewFloat4(const int id);
	static UiNode createRandFloat3(const int id);
	static UiNode createAddFloat3(const int id);
	static UiNode createMultiplyFloat3ByScalar(const int id);
	static UiNode createGetParticlePosition(const int id);
	static UiNode createGetParticleVelocity(const int id);
	static UiNode createGetParticleAcceleration(const int id);
	static UiNode createEmitterOutput(const int id);
	static UiNode createSimulatorOutput(const int id);
	static UiNode createRendererOutput(const int id);
	static UiNode createStartParticleSimulation(const int id);
	static UiNode createPointAttractionForce(const int id);
	static UiNode createDragForce(const int id);
	static UiNode createVortexForce(const int id);
	static UiNode createCurlNoiseForce(const int id);
	static UiNode createSampleTexture2d(const int id);
};