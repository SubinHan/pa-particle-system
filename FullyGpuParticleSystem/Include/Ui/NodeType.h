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
	EmitterOutput,
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
	NodeType::EmitterOutput,
};

const std::array<std::string, static_cast<size_t>(NodeType::Size)> nodeNames =
{
	"Empty",
	"NewFloat",
	"NewFloat3",
	"NewFloat4",
	"RandFloat3",
	"AddFloat3",
	"EmitterOutput",
};