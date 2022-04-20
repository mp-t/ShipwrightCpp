#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>

#include <cstdint>
#include <vector>

namespace Ship
{

class PlayerAnimationV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class PlayerAnimation : public Resource
{
public:
	std::vector<std::int16_t> limbRotData;
};

}