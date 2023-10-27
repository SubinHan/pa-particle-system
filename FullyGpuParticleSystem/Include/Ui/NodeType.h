#pragma once

#include <array>

enum class NodeType
{
	Empty,
	NewFloat,
	NewFloat3,
	NewFloat4,
	RandFloat3,
	AddFloat3,
	GetParticlePosition,
	GetParticleVelocity,
	GetParticleAcceleration,
	EmitterOutput,
	SimulatorOutput,
	RendererOutput,
	StartParticleSimulation,
	PointAttractionForce,
	DragForce,
	VortexForce,
	Size
};

const std::array<NodeType, static_cast<size_t>(NodeType::Size)> nodeTypes = 
{
	NodeType::Empty,
	NodeType::NewFloat,
	NodeType::NewFloat3,
	NodeType::NewFloat4,
	NodeType::RandFloat3,
	NodeType::AddFloat3,
	NodeType::GetParticlePosition,
	NodeType::GetParticleVelocity,
	NodeType::GetParticleAcceleration,
	NodeType::EmitterOutput,
	NodeType::SimulatorOutput,
	NodeType::RendererOutput,
	NodeType::StartParticleSimulation,
	NodeType::PointAttractionForce,
	NodeType::DragForce,
	NodeType::VortexForce,
};

const std::array<std::string, static_cast<size_t>(NodeType::Size)> nodeNames =
{
	"Empty",
	"NewFloat",
	"NewFloat3",
	"NewFloat4",
	"RandFloat3",
	"AddFloat3",
	"GetParticlePosition",
	"GetParticleVelocity",
	"GetParticleAcceleration",
	"EmitterOutput",
	"SimulatorOutput",
	"RendererOutput",
	"StartParticleSimulation",
	"PointAttractionForce",
	"DragForce",
	"VortexForce",
};