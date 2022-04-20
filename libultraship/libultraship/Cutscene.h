#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>

#include <cstdint>
#include <vector>

namespace Ship
{

class CutsceneV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

struct CutsceneCommand
{
	std::uint32_t commandID;
	std::uint32_t commandIndex;
};

class Cutscene : public Resource
{
public:
	std::vector<std::uint32_t> commands;
};

}