#include "Animation.h"

void Ship::AnimationV0::ParseFileBinary(BinaryReader* reader, Resource* res)
{
	Animation* anim = (Animation*)res;

	ResourceFile::ParseFileBinary(reader, res);

	AnimationType animType = (AnimationType)reader->ReadUInt32();
	anim->type = animType;

	if (animType == AnimationType::Normal)
	{
		anim->frameCount = reader->ReadInt16();

		std::uint32_t rotValuesCnt = reader->ReadUInt32();
		anim->rotationValues.reserve(rotValuesCnt);
		for (std::uint32_t i = 0; i < rotValuesCnt; i++)
			anim->rotationValues.push_back(reader->ReadUInt16());


		std::uint32_t rotIndCnt = reader->ReadUInt32();
		anim->rotationIndices.reserve(rotIndCnt);
		for (std::uint32_t i = 0; i < rotIndCnt; i++)
		{
			std::uint16_t x = reader->ReadUInt16();
			std::uint16_t y = reader->ReadUInt16();
			std::uint16_t z = reader->ReadUInt16();
			anim->rotationIndices.push_back(RotationIndex(x, y, z));
		}
		anim->limit = reader->ReadInt16();
	}
	else if (animType == AnimationType::Curve)
	{
		anim->frameCount = reader->ReadInt16();

		std::uint32_t refArrCnt = reader->ReadUInt32();
		anim->refIndexArr.reserve(refArrCnt);
		for (std::uint32_t i = 0; i < refArrCnt; i++)
			anim->refIndexArr.push_back(reader->ReadUByte());

		std::uint32_t transformDataCnt = reader->ReadUInt32();
		anim->transformDataArr.reserve(transformDataCnt);
		for (std::uint32_t i = 0; i < transformDataCnt; i++)
		{
			TransformData data;
			data.unk_00 = reader->ReadUInt16();
			data.unk_02 = reader->ReadInt16();
			data.unk_04 = reader->ReadInt16();
			data.unk_06 = reader->ReadInt16();
			data.unk_08 = reader->ReadSingle();

			anim->transformDataArr.push_back(data);
		}

		std::uint32_t copyValuesCnt = reader->ReadUInt32();
		anim->copyValuesArr.reserve(copyValuesCnt);
		for (std::uint32_t i = 0; i < copyValuesCnt; i++)
		{
			anim->copyValuesArr.push_back(reader->ReadInt16());
		}
	}
	else if (animType == AnimationType::Link)
	{
		anim->frameCount = reader->ReadInt16();
		anim->segPtr = reader->ReadUInt32();
	}
	else if (animType == AnimationType::Legacy)
	{
		SPDLOG_DEBUG("BEYTAH ANIMATION?!");
	}
}
