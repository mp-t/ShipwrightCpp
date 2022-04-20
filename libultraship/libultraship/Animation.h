#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>

#include <cstdint>
#include <vector>

namespace Ship
{

enum class AnimationType
{
	Normal = 0,
	Link = 1,
	Curve = 2,
	Legacy = 3,
};

struct RotationIndex
{
	std::uint16_t x;
	std::uint16_t y;
	std::uint16_t z;
};

struct TransformData
{
	///* 0x0000 */ u16 unk_00; // appears to be flags
	std::uint16_t unk_00;
	///* 0x0002 */ s16 unk_02;
	std::int16_t unk_02;
	///* 0x0004 */ s16 unk_04;
	std::int16_t unk_04;
	///* 0x0006 */ s16 unk_06;
	std::int16_t unk_06;
	///* 0x0008 */ f32 unk_08;
	float unk_08;
};

class AnimationV0 : public ResourceFile
{
public:
	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class Animation : public Resource
{
public:
	AnimationType type;
	std::int16_t frameCount;

	// NORMAL
	std::vector<std::uint16_t> rotationValues;
	std::vector<RotationIndex> rotationIndices;
	std::int16_t limit = 0;

	// CURVE
	std::vector<std::uint8_t> refIndexArr;
	std::vector<TransformData> transformDataArr;
	std::vector<std::int16_t> copyValuesArr;

	// LINK
	std::uint32_t segPtr; // This is temp
};

}