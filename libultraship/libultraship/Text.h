#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>

#include <cstdint>
#include <string>
#include <vector>

namespace Ship
{

struct MessageEntry
{
	std::uint16_t id;
	std::uint8_t textboxType;
	std::uint8_t textboxYPos;
	std::string msg;
};

class TextV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class Text : public Resource
{
public:
	std::vector<MessageEntry> messages;
};

}