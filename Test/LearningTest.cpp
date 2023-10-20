#include "pch.h"
#include "CppUnitTest.h"

#include <chrono>
#include <functional>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Test
{
	TEST_CLASS(LearningTest)
	{
	public:

		TEST_METHOD(LearnChrono)
		{
			auto start = std::chrono::high_resolution_clock::now();
			auto end = std::chrono::high_resolution_clock::now();

			Logger::WriteMessage(std::to_string(
				std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()).c_str());
		}

		TEST_METHOD(LearnHash)
		{
			auto timePoint1 = std::chrono::high_resolution_clock::now();
			auto timePoint2 = std::chrono::high_resolution_clock::now();
			Logger::WriteMessage(std::to_string(
				std::hash<size_t>{}(timePoint1.time_since_epoch().count())).c_str());
			Logger::WriteMessage("\n");
			Logger::WriteMessage(std::to_string(
				std::hash<size_t>{}(timePoint2.time_since_epoch().count())).c_str());
		}
	};
}
