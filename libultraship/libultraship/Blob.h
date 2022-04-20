#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>

#include <cstdint>
#include <vector>

namespace Ship
{

class BlobV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class Blob : public Resource
{
public:
	std::vector<std::uint8_t> data;
};

};