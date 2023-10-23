#include "pch.h"
#include "CppUnitTest.h"
#include "../FullyGpuParticleSystem/Include/Core/HlslTranslator.h"
#include "../FullyGpuParticleSystem/Include/Core/HlslTranslatorEmit.h"

#include <fstream>
#include <Windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(ShaderGenerationTest)
	{
	public:
		
		TEST_METHOD(FIleIoTest)
		{
			const std::wstring OUTPUT_PATH = PROJECT_DIR + L"TestOutput/Test.hlsl";
			std::string outputPathString;
			outputPathString.assign(OUTPUT_PATH.begin(), OUTPUT_PATH.end());
			
			remove(outputPathString.c_str());

			HlslTranslator translator(PROJECT_DIR + L"FullyGpuParticleSystem/ParticleSystemShaders/ParticleEmitCSBase.hlsl");
			translator.compile(OUTPUT_PATH);

			std::ifstream fin;
			fin.open(OUTPUT_PATH);
			
			// assert
			Assert::IsTrue(fin.is_open());

			constexpr UINT BUFFER_SIZE = 512;
			char buffer[BUFFER_SIZE];
			fin.getline(buffer, BUFFER_SIZE);
			Assert::AreEqual(buffer[0], '#');

			fin.close();
		}

		TEST_METHOD(GenerateSimple)
		{
			const std::wstring OUTPUT_PATH = PROJECT_DIR + L"TestOutput/TestSimple.hlsl";
			std::string outputPathString;
			outputPathString.assign(OUTPUT_PATH.begin(), OUTPUT_PATH.end());

			remove(outputPathString.c_str());

			HlslTranslator translator(PROJECT_DIR + L"FullyGpuParticleSystem/ParticleSystemShaders/ParticleEmitCSBase.hlsl");
			UINT float4Index = translator.newFloat4(1.0f, 1.0f, 1.0f, 1.0f);

			translator.compile(OUTPUT_PATH);

			// assert
			std::ifstream fin;
			fin.open(OUTPUT_PATH);

			constexpr UINT BUFFER_SIZE = 512;
			char buffer[BUFFER_SIZE];

			bool isContainsFormatter = false;
			bool didGenerateFloat4 = false;

			while (fin.getline(buffer, BUFFER_SIZE))
			{
				std::string line(buffer);

				if (line.find("%s") != std::string::npos)
				{
					isContainsFormatter = true;
				}

				if (line.find("float4 local0") != std::string::npos)
				{
					didGenerateFloat4 = true;
				}
			}

			Assert::IsFalse(isContainsFormatter);
			Assert::IsTrue(didGenerateFloat4);
		}

		TEST_METHOD(GenerateDependentVariables)
		{
			const std::wstring OUTPUT_PATH = PROJECT_DIR + L"TestOutput/TestDependency.hlsl";
			std::string outputPathString;
			outputPathString.assign(OUTPUT_PATH.begin(), OUTPUT_PATH.end());

			remove(outputPathString.c_str());

			HlslTranslator translator(PROJECT_DIR + L"FullyGpuParticleSystem/ParticleSystemShaders/ParticleEmitCSBase.hlsl");
			UINT float4Index1 = translator.newFloat4(1.0f, 1.0f, 1.0f, 1.0f);
			UINT float4Index2 = translator.randFloat4();
			UINT float4Index5 = translator.newFloat1(1.0f);
			UINT float4Index3 = translator.setAlpha(float4Index2, float4Index5);
			UINT float4Index4 = translator.addFloat4(float4Index1, float4Index3);

			translator.compile(OUTPUT_PATH);

			// assert
			std::ifstream fin;
			fin.open(OUTPUT_PATH);

			constexpr UINT BUFFER_SIZE = 512;
			char buffer[BUFFER_SIZE];

			bool isContainsFormatter = false;
			bool didGenerateLocal0 = false;
			bool didGenerateLocal1 = false;
			bool didGenerateLocal2 = false;
			bool didGenerateLocal3 = false;

			while (fin.getline(buffer, BUFFER_SIZE))
			{
				std::string line(buffer);

				if (line.find("%s") != std::string::npos)
				{
					isContainsFormatter = true;
				}

				if (line.find("float4 local0") != std::string::npos)
				{
					didGenerateLocal0 = true;
				}

				if (line.find("float4 local1") != std::string::npos)
				{
					didGenerateLocal1 = true;
				}

				if (line.find("float4 local2") != std::string::npos)
				{
					didGenerateLocal2 = true;
				}

				if (line.find("float4 local3") != std::string::npos)
				{
					didGenerateLocal3 = true;
				}
			}

			Assert::IsFalse(isContainsFormatter);
			Assert::IsTrue(didGenerateLocal0);
			Assert::IsTrue(didGenerateLocal1);
			Assert::IsTrue(didGenerateLocal2);
			Assert::IsTrue(didGenerateLocal3);
		}

		TEST_METHOD(GenerateEmitCS)
		{

			const std::wstring OUTPUT_PATH = PROJECT_DIR + L"TestOutput/TestEmitCS.hlsl";
			std::string outputPathString;
			outputPathString.assign(OUTPUT_PATH.begin(), OUTPUT_PATH.end());

			remove(outputPathString.c_str());

			HlslTranslatorEmit translator(PROJECT_DIR + L"FullyGpuParticleSystem/ParticleSystemShaders/ParticleEmitCSBase.hlsl");
			UINT positionIndex = translator.newFloat3(0.0f, 0.0f, 0.0f);
			UINT velocityIndex = translator.randFloat3();
			UINT accelerationIndex = translator.newFloat3(0.0f, -0.1f, 0.0f);
			UINT lifetimeIndex = translator.newFloat1(4.0f);
			UINT sizeIndex = translator.newFloat1(0.05f);
			UINT opacityIndex = translator.newFloat1(1.0f);

			translator.setInitialPosition(positionIndex);
			translator.setInitialVelocity(velocityIndex);
			translator.setInitialAcceleration(accelerationIndex);
			translator.setInitialLifetime(lifetimeIndex);
			translator.setInitialSize(sizeIndex);
			translator.setInitialOpacity(opacityIndex);

			translator.compile(OUTPUT_PATH);

			// assert
			std::ifstream fin;
			fin.open(OUTPUT_PATH);

			constexpr UINT BUFFER_SIZE = 512;
			char buffer[BUFFER_SIZE];

			bool isContainsFormatter = false;
			bool didGenerateLocal0 = false;
			bool didGenerateLocal1 = false;
			bool didGenerateLocal2 = false;
			bool didGenerateLocal3 = false;
			bool didGenerateLocal4 = false;
			bool didGenerateLocal5 = false;

			while (fin.getline(buffer, BUFFER_SIZE))
			{
				std::string line(buffer);

				if (line.find("%s") != std::string::npos)
				{
					isContainsFormatter = true;
				}

				if (line.find("float3 local0") != std::string::npos)
				{
					didGenerateLocal0 = true;
				}

				if (line.find("float3 local1") != std::string::npos)
				{
					didGenerateLocal1 = true;
				}

				if (line.find("float3 local2") != std::string::npos)
				{
					didGenerateLocal2 = true;
				}

				if (line.find("float local3") != std::string::npos)
				{
					didGenerateLocal3 = true;
				}

				if (line.find("float local4") != std::string::npos)
				{
					didGenerateLocal4 = true;
				}

				if (line.find("float local5") != std::string::npos)
				{
					didGenerateLocal5 = true;
				}
			}

			Assert::IsFalse(isContainsFormatter);
			Assert::IsTrue(didGenerateLocal0);
			Assert::IsTrue(didGenerateLocal1);
			Assert::IsTrue(didGenerateLocal2);
			Assert::IsTrue(didGenerateLocal3);
			Assert::IsTrue(didGenerateLocal4);
			Assert::IsTrue(didGenerateLocal5);
		}
	};
}
