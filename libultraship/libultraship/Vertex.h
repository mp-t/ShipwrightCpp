#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>

#include <vector>

namespace Ship
{

struct Vtx
{
	std::int16_t x, y, z;
	std::uint16_t flag;
	std::int16_t s, t;
	std::uint8_t r, g, b, a;
};

class VertexV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class Vertex : public Resource
{
public:
	std::vector<Vtx> vtxList;
};

}
