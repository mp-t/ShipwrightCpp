#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>
#include <Vec3s.h>

#include <vector>

namespace Ship
{

class PathV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class Path : public Resource
{
public:
	std::vector<std::vector<Vec3s>> paths;
};

}