#include "Cutscene.h"

void Ship::CutsceneV0::ParseFileBinary(BinaryReader* reader, Resource* res)
{
	Cutscene* cs = (Cutscene*)res;

	ResourceFile::ParseFileBinary(reader, res);

	std::uint32_t numEntries = reader->ReadUInt32();
	cs->commands.reserve(numEntries);

	for (std::uint32_t i = 0; i < numEntries; i++)
	{
		std::uint32_t data = reader->ReadUInt32();

		cs->commands.push_back(data);
	}

	//int bp = 0;
}
