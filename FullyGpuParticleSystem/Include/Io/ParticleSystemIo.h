#pragma once

#include <vector>
#include <string>
#include <filesystem>

class ParticleSystem;

class ParticleSystemIo
{
	// [filepath, particleSystmeName]
	using ParticleSystemFile = std::pair<std::string, std::string>;

public:
	static std::vector<ParticleSystemFile> getSavedParticleSystems(std::string directory);

	static void save(std::string directory, ParticleSystem* particleSystem);
	static void loadInto(std::string filePath, ParticleSystem* particleSystem);

private:
	static bool isSaveFile(std::filesystem::path path);
	static bool isDefaultSaveFile(std::string filename);
	static bool isEditorSaveFile(std::string particleSystemName, std::filesystem::path filepath);
	static bool endsWith(std::string str, std::string suffix);
	static bool startsWith(std::string str, std::string prefix);
	static std::string getParticleSystemNameByFilename(std::string filename);
};