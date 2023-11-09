#include "Ui/UiNodeFactory.h"

#include "Ui/NodeType.h"
#include "Ui/ValueType.h"

#include <cassert>

UiNode UiNodeFactory::createNode(const int id, NodeType nodeType)
{
	switch (nodeType)
	{
	case NodeType::Empty:
		return createEmpty(id);
	case NodeType::NewFloat:
		return createNewFloat(id);
	case NodeType::NewFloat3:
		return createNewFloat3(id);
	case NodeType::NewFloat4:
		return createNewFloat4(id);
	case NodeType::RandFloat:
		return createRandFloat(id);
	case NodeType::RandFloat3:
		return createRandFloat3(id);
	case NodeType::AddFloat:
		return createAddFloat(id);
	case NodeType::AddFloat3:
		return createAddFloat3(id);
	case NodeType::MultiplyFloat:
		return createMultiplyFloat(id);
	case NodeType::MultiplyFloat3ByScalar:
		return createMultiplyFloat3ByScalar(id);
	case NodeType::MaskX:
		return createMaskX(id);
	case NodeType::MaskY:
		return createMaskY(id);
	case NodeType::MaskZ:
		return createMaskZ(id);
	case NodeType::MaskW:
		return createMaskW(id);
	case NodeType::SinTime:
		return createSinByTime(id);
	case NodeType::GetParticlePosition:
		return createGetParticlePosition(id);
	case NodeType::GetParticleVelocity:
		return createGetParticleVelocity(id);
	case NodeType::GetParticleAcceleration:
		return createGetParticleAcceleration(id);
	case NodeType::GetParticleColor:
		return createGetParticleColor(id);
	case NodeType::GetParticleAlpha:
		return createGetParticleAlpha(id);
	case NodeType::EmitterOutput:
		return createEmitterOutput(id);
	case NodeType::SimulatorOutput:
		return createSimulatorOutput(id);
	case NodeType::RendererOutput:
		return createRendererOutput(id);
	case NodeType::StartParticleSimulation:
		return createStartParticleSimulation(id);
	case NodeType::PointAttractionForce:
		return createPointAttractionForce(id);
	case NodeType::DragForce:
		return createDragForce(id);
	case NodeType::VortexForce:
		return createVortexForce(id);
	case NodeType::CurlNoiseForce:
		return createCurlNoiseForce(id);
	case NodeType::SampleTexture2d:
		return createSampleTexture2d(id);
	case NodeType::GrayscaleToTranslucent:
		return createGrayscaleToTranslucent(id);
	case NodeType::SetColorOfFloat4:
		return createSetColorOfFloat4(id);
	case NodeType::SampleTexture2dSubUvAnimation:
		return createSampleTexture2dSubUvAnimation(id);
	case NodeType::MakeFloat4ByColorAlpha:
		return createMakeFloat4ByColorAlpha(id);
	case NodeType::MakeFloat3:
		return createMakeFloat3(id);
	case NodeType::MakeFloat4:
		return createMakeFloat4(id);
	default:
		// should never reach here.
		assert(0 && "Unknown node type was given.");
	}

	return UiNode(
		-1, 
		"error",
		std::vector<std::string>(), 
		std::vector<std::string>(), 
		std::vector<ValueType>(), 
		std::vector<std::string>(), 
		NodeType::Size);
}

UiNode UiNodeFactory::createEmpty(const int id)
{
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;

	return UiNode(id, "Empty", emptyVector, emptyVector, emptyValueType, emptyVector, NodeType::Empty);
}

UiNode UiNodeFactory::createNewFloat(const int id)
{
	constexpr auto nodeType = NodeType::NewFloat;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantInputNames =
	{
		"r",
	};
	std::vector<ValueType> constantsValueTypes(constantInputNames.size(), ValueType::Float);
	std::vector<std::string> outputNames =
	{
		"float"
	};
	std::string nodeName = nodeNames[static_cast<int>(nodeType)];

	return UiNode(id, nodeName, emptyVector, constantInputNames, constantsValueTypes, outputNames, nodeType);
}

UiNode UiNodeFactory::createNewFloat3(const int id)
{
	constexpr auto nodeType = NodeType::NewFloat3;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantInputNames =
	{
		"r",
		"g",
		"b",
	};
	std::vector<ValueType> constantsValueTypes(constantInputNames.size(), ValueType::Float);
	std::vector<std::string> outputNames =
	{
		"float3"
	};
	std::string nodeName = nodeNames[static_cast<int>(nodeType)];

	return UiNode(id, nodeName, emptyVector, constantInputNames, constantsValueTypes, outputNames, nodeType);
}

UiNode UiNodeFactory::createNewFloat4(const int id)
{
	constexpr auto nodeType = NodeType::NewFloat4;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantInputNames =
	{
		"r",
		"g",
		"b",
		"a",
	};
	std::vector<ValueType> constantsValueTypes(constantInputNames.size(), ValueType::Float);
	std::vector<std::string> outputNames =
	{
		"float4"
	};
	std::string nodeName = nodeNames[static_cast<int>(nodeType)];

	return UiNode(id, nodeName, emptyVector, constantInputNames, constantsValueTypes, outputNames, nodeType);
}

UiNode UiNodeFactory::createRandFloat(const int id)
{
	constexpr auto nodeType = NodeType::RandFloat;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createRandFloat3(const int id)
{
	constexpr auto nodeType = NodeType::RandFloat3;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createAddFloat(const int id)
{
	constexpr auto nodeType = NodeType::AddFloat;
	std::vector<std::string> inputNames =
	{
		"floatInput0",
		"floatInput1",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createAddFloat3(const int id)
{
	constexpr auto nodeType = NodeType::AddFloat3;
	std::vector<std::string> inputNames = 
	{
		"float3input0",
		"float3input1",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMultiplyFloat3ByScalar(const int id)
{
	constexpr auto nodeType = NodeType::MultiplyFloat3ByScalar;
	std::vector<std::string> inputNames =
	{
		"float3input0",
		"floatinput1",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createGetParticlePosition(const int id)
{
	constexpr auto nodeType = NodeType::GetParticlePosition;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createGetParticleVelocity(const int id)
{
	constexpr auto nodeType = NodeType::GetParticleVelocity;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createGetParticleAcceleration(const int id)
{
	constexpr auto nodeType = NodeType::GetParticleAcceleration;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createEmitterOutput(const int id)
{
	constexpr auto nodeType = NodeType::EmitterOutput;
	std::vector<std::string> inputNames =
	{
		"initialPosition",
		"initialVelocity",
		"initialAcceleration",
		"initialLifetime",
		"initialSize",
		"initialOpacity",
		"InitialColor",
		"EndSize",
		"EndOpacity",
		"EndColor"
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, emptyVector, nodeType);
}

UiNode UiNodeFactory::createSimulatorOutput(const int id)
{
	constexpr auto nodeType = NodeType::SimulatorOutput;
	std::vector<std::string> inputNames =
	{
		"finalParticleResult"
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, emptyVector, nodeType);
}

UiNode UiNodeFactory::createRendererOutput(const int id)
{
	constexpr auto nodeType = NodeType::RendererOutput;
	std::vector<std::string> inputNames =
	{
		"outputColor"
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, emptyVector, nodeType);
}

UiNode UiNodeFactory::createStartParticleSimulation(const int id)
{
	constexpr auto nodeType = NodeType::StartParticleSimulation;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"particleState"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createPointAttractionForce(const int id)
{
	constexpr auto nodeType = NodeType::PointAttractionForce;
	std::vector<std::string> inputOutputNames =
	{
		"particleState"
	};
	std::vector<std::string> constantsNames = 
	{
		"pointX",
		"pointY",
		"pointZ",
		"radius",
		"strength",
	};
	std::vector<ValueType> constantsValueTypes(constantsNames.size(), ValueType::Float);

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputOutputNames, constantsNames, constantsValueTypes, inputOutputNames, nodeType);
}

UiNode UiNodeFactory::createDragForce(const int id)
{
	constexpr auto nodeType = NodeType::DragForce;
	std::vector<std::string> inputOutputNames =
	{
		"particleState"
	};
	std::vector<std::string> constantsNames =
	{
		"dragCoefficient"
	};
	std::vector<ValueType> constantsValueTypes(constantsNames.size(), ValueType::Float);

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputOutputNames, constantsNames, constantsValueTypes, inputOutputNames, nodeType);
}

UiNode UiNodeFactory::createVortexForce(const int id)
{
	constexpr auto nodeType = NodeType::VortexForce;
	std::vector<std::string> inputOutputNames =
	{
		"particleState"
	};
	std::vector<std::string> constantsNames =
	{
		"vortexCenterX",
		"vortexCenterY",
		"vortexCenterZ",
		"vortexAxisX",
		"vortexAxisY",
		"vortexAxisZ",
		"magnitude",
		"tightness",
	};
	std::vector<ValueType> constantsValueTypes(constantsNames.size(), ValueType::Float);

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputOutputNames, constantsNames, constantsValueTypes, inputOutputNames, nodeType);
}

UiNode UiNodeFactory::createCurlNoiseForce(const int id)
{
	constexpr auto nodeType = NodeType::CurlNoiseForce;
	std::vector<std::string> inputOutputNames =
	{
		"particleState"
	};
	std::vector<std::string> constantsNames =
	{
		"amplitude",
		"frequency",
	};
	std::vector<ValueType> constantsValueTypes =
	{
		ValueType::Float,
		ValueType::Float,
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputOutputNames, constantsNames, constantsValueTypes, inputOutputNames, nodeType);
}

UiNode UiNodeFactory::createSampleTexture2d(const int id)
{
	constexpr auto nodeType = NodeType::SampleTexture2d;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantsNames =
	{
		"textureName",
	};
	std::vector<ValueType> constantsValueTypes =
	{
		ValueType::String,
	};

	std::vector<std::string> outputNames =
	{
		"float4",
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, constantsNames, constantsValueTypes, outputNames, nodeType);
}

UiNode UiNodeFactory::createGrayscaleToTranslucent(const int id)
{
	constexpr auto nodeType = NodeType::GrayscaleToTranslucent;
	std::vector<std::string> inputNames =
	{
		"grayscaleColorFloat4"
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float4",
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createSetColorOfFloat4(const int id)
{
	constexpr auto nodeType = NodeType::SetColorOfFloat4;
	std::vector<std::string> inputNames =
	{
		"float4",
		"color"
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float4",
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createGetParticleColor(const int id)
{
	constexpr auto nodeType = NodeType::GetParticleColor;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createGetParticleAlpha(const int id)
{
	constexpr auto nodeType = NodeType::GetParticleAlpha;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createSampleTexture2dSubUvAnimation(const int id)
{
	constexpr auto nodeType = NodeType::SampleTexture2dSubUvAnimation;
	std::vector<std::string> emptyVector;
	std::vector<std::string> constantsNames =
	{
		"textureName",
		"numSubTexturesX",
		"numSubTexturesY",
	};
	std::vector<ValueType> constantsValueTypes =
	{
		ValueType::String,
		ValueType::Float,
		ValueType::Float,
	};

	std::vector<std::string> outputNames =
	{
		"float4",
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, constantsNames, constantsValueTypes, outputNames, nodeType);
}

UiNode UiNodeFactory::createMultiplyFloat(const int id)
{
	constexpr auto nodeType = NodeType::MultiplyFloat;
	std::vector<std::string> inputNames =
	{
		"floatInput0",
		"floatInput1",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMakeFloat4ByColorAlpha(const int id)
{
	constexpr auto nodeType = NodeType::MakeFloat4ByColorAlpha;
	std::vector<std::string> inputNames =
	{
		"float3Input",
		"floatInput",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float4"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMakeFloat3(const int id)
{
	constexpr auto nodeType = NodeType::MakeFloat3;
	std::vector<std::string> inputNames =
	{
		"floatX",
		"floatY",
		"floatZ",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float3"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMakeFloat4(const int id)
{
	constexpr auto nodeType = NodeType::MakeFloat4;
	std::vector<std::string> inputNames =
	{
		"floatX",
		"floatY",
		"floatZ",
		"floatW",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float4"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMaskX(const int id)
{
	constexpr auto nodeType = NodeType::MaskX;
	std::vector<std::string> inputNames =
	{
		"floatInput",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMaskY(const int id)
{
	constexpr auto nodeType = NodeType::MaskY;
	std::vector<std::string> inputNames =
	{
		"floatInput",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMaskZ(const int id)
{
	constexpr auto nodeType = NodeType::MaskZ;
	std::vector<std::string> inputNames =
	{
		"floatInput",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createMaskW(const int id)
{
	constexpr auto nodeType = NodeType::MaskW;
	std::vector<std::string> inputNames =
	{
		"floatInput",
	};
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, inputNames, emptyVector, emptyValueType, outputNames, nodeType);
}

UiNode UiNodeFactory::createSinByTime(const int id)
{
	constexpr auto nodeType = NodeType::SinTime;
	std::vector<std::string> emptyVector;
	std::vector<ValueType> emptyValueType;
	std::vector<std::string> outputNames =
	{
		"float"
	};

	std::string nodeName = nodeNames[static_cast<int>(nodeType)];
	return UiNode(id, nodeName, emptyVector, emptyVector, emptyValueType, outputNames, nodeType);
}
