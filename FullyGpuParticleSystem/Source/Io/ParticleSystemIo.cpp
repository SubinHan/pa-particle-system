#include "Io/ParticleSystemIo.h"

#include "Core/ParticleSystem.h"
#include "Core/ParticleEmitter.h"
#include "Core/ParticleSimulator.h"
#include "Core/HlslTranslatorEmit.h"
#include "Core/HlslTranslatorSimulate.h"
#include "Core/HlslTranslatorRenderPs.h"
#include "Io/NodeEditorIo.h"

#include <fstream>
#include <filesystem>

const std::string FILE_EXTENSION = ".ps";

std::vector<ParticleSystemIo::ParticleSystemFile> ParticleSystemIo::getSavedParticleSystems(std::string directory)
{
	std::vector<ParticleSystemIo::ParticleSystemFile> result;

	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.is_directory())
			continue;

		auto filename = entry.path().filename().string();

		if (isDefaultSaveFile(filename))
			continue;

		if (!isSaveFile(filename))
			continue;

		result.push_back(std::make_pair<std::string, std::string>(
			entry.path().string(), getParticleSystemNameByFilename(filename)));
	}

	return result;
}

void ParticleSystemIo::save(std::string directory, ParticleSystem* particleSystem)
{
	std::string filePath = directory + particleSystem->getName() + FILE_EXTENSION;

	std::fstream fout(
		filePath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	float spawnRate = particleSystem->getSpawnRate();
	fout.write(
		reinterpret_cast<const char*>(&spawnRate),
		static_cast<std::streamsize>(sizeof(decltype(spawnRate))));
}

// should correct particleSystem name.
void ParticleSystemIo::loadInto(std::string filePath, ParticleSystem* particleSystem)
{
	std::fstream fin(
		filePath, std::ios_base::in | std::ios_base::binary);

	float spawnRate;
	fin.read(
		reinterpret_cast<char*>(&spawnRate),
		static_cast<std::streamsize>(sizeof(decltype(spawnRate))));

	particleSystem->setSpawnRate(spawnRate);

	// load emitter, simulator, renderer shaders.
	std::filesystem::path p(filePath);
	auto directory = p.parent_path();

	bool foundEmitterEditorSaveFile = false;
	bool foundSimulatorEditorSaveFile = false;
	bool foundRendererEditorSaveFile = false;

	for (const auto& entry : std::filesystem::directory_iterator(directory))
	{
		if (entry.is_directory())
			continue;

		auto filename = entry.path().filename();
		std::string filenameWithoutExtension = filename.filename().replace_extension().string();

		if (isEditorSaveFile(particleSystem->getName(), filename))
		{
			auto filepath = directory / filename.filename().replace_extension().string();
			
			auto [nodes, links, currentId] = NodeEditorIo::load(filepath);

			if (!foundEmitterEditorSaveFile && endsWith(filenameWithoutExtension, "Emitter"))
			{
				HlslTranslatorEmit translator(nodes, links);
				translator.translateTo(particleSystem->getEmitter());
				foundEmitterEditorSaveFile = true;
			}
			else if (!foundSimulatorEditorSaveFile && endsWith(filenameWithoutExtension, "Simulator"))
			{
				HlslTranslatorSimulate translator(nodes, links);
				translator.translateTo(particleSystem->getSimulator());
				foundSimulatorEditorSaveFile = true;
			}
			else if (!foundRendererEditorSaveFile && endsWith(filenameWithoutExtension, "Renderer"))
			{
				auto renderer = particleSystem->getRenderer();
				HlslTranslatorRenderPs translator(nodes, links, renderer);
				translator.translateTo(renderer);
				foundRendererEditorSaveFile = true;
			}
		}
	}

	if (!foundEmitterEditorSaveFile)
	{
		// TODO
	}

	if (!foundSimulatorEditorSaveFile)
	{
		// TODO
	}

	if (!foundRendererEditorSaveFile)
	{
		// TODO
	}
}

bool ParticleSystemIo::isSaveFile(std::filesystem::path path)
{
	auto a = path.filename();
	auto b = path.filename().extension();

	return path.filename().extension().string() == FILE_EXTENSION;
}

bool ParticleSystemIo::isDefaultSaveFile(std::string filename)
{
	return startsWith(filename, "__Default");
}

bool ParticleSystemIo::isEditorSaveFile(std::string particleSystemName, std::filesystem::path filepath)
{
	std::string filenameWithoutExtension = filepath.filename().replace_extension().string();

	if (filenameWithoutExtension.find(particleSystemName + "_") != std::string::npos)
		return true;

	return false;
}

bool ParticleSystemIo::endsWith(std::string str, std::string suffix)
{
	if (str.length() < suffix.length())
		return false;

	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

bool ParticleSystemIo::startsWith(std::string str, std::string prefix)
{
	if (str.length() < prefix.length())
		return false;

	return std::equal(prefix.begin(), prefix.end(), str.begin());
}

std::string ParticleSystemIo::getParticleSystemNameByFilename(std::string filename)
{
	return filename.substr(0, filename.size() - FILE_EXTENSION.size());
}
