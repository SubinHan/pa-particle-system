#include "Core/MaterialManager.h"

#include <memory>

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager() = default;

MaterialManager* MaterialManager::getInstance()
{
	static std::unique_ptr<MaterialManager> instance = 
		std::unique_ptr<MaterialManager>{new MaterialManager()};
	return instance.get();
}

void MaterialManager::addMaterial(Material material)
{
	_materials.push_back(material);
}

Material MaterialManager::getMaterial(std::string name)
{
	for (auto material : _materials)
	{
		if (material.Name == name)
			return material;
	}

	assert(0, "There is no material that has given name.");
	return Material();
}
