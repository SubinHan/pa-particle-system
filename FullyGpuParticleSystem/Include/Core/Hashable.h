#pragma once

class Hashable
{
public:
	Hashable();
	virtual ~Hashable() = default;

	size_t getHash() const;

protected:
	const size_t _hash;

private:
	size_t calculateHash() const;
};