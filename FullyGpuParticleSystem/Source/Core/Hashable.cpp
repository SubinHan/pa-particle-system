#include "Core/Hashable.h"

#include <chrono>

Hashable::Hashable() :
	_hash(calculateHash())
{
}

size_t Hashable::getHash() const
{
	return _hash;
}

size_t Hashable::calculateHash() const
{
	auto creationTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	return std::hash<size_t>{}(creationTime);
}
