#pragma once

#include "Resource.h"

namespace Ship
{
	class CutsceneV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class CutsceneCommand
	{
	public:
		std::uint32_t commandID;
		std::uint32_t commandIndex;

		CutsceneCommand() {};
	};

	class Cutscene : public Resource
	{
	public:
		//std::int32_t endFrame;
		std::vector<std::uint32_t> commands;
	};
}