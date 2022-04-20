#pragma once

#include "Resource.h"

#include <StrHash.h>
#include <Utils/BinaryReader.h>

#include <cstdint>
#include <vector>

namespace Ship
{

enum class MaterialCmt
{
	Wrap = 0,
	Mirror = 1,
	Clamp = 2
};

class ShaderParam
{
public:
	strhash name;
	DataType dataType;
	std::uint64_t value;

	explicit ShaderParam(BinaryReader* reader);
};

class MaterialV0 : public ResourceFile
{
public:
	// Typical N64 Stuff
	MaterialCmt cmtH, cmtV;
	std::uint8_t clrR, clrG, clrB, clrA, clrM, clrL;

	// Modern Stuff
	strhash shaderID;
	std::uint32_t shaderParamsCnt;
	std::uint32_t offsetToShaderEntries;

	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class Material : public Resource
{
public:
	// Typical N64 Stuff
	MaterialCmt cmtH, cmtV;
	std::uint8_t clrR, clrG, clrB, clrA, clrM, clrL;

	// Modern Stuff
	strhash shaderID;
	std::vector<ShaderParam*> shaderParams;
};

}