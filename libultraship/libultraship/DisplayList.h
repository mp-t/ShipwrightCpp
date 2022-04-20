#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>

#include <cstdint>
#include <vector>

namespace Ship
{

class DisplayListV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class DisplayList : public Resource
{
public:
	std::vector<std::uint64_t> instructions;
};

}