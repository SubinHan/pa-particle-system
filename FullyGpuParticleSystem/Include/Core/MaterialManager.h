#pragma once

#include "Model/Material.h"

#include <vector>
#include <string>

class MaterialManager
{
public:
	~MaterialManager();

	static MaterialManager* getInstance();

	void addMaterial(Material material);
	Material getMaterial(std::string name);

private:
	MaterialManager();

private:
	std::vector<Material> _materials;
};