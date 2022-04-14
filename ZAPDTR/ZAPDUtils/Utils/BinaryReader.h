#pragma once

#include "Color3b.h"
#include "Utils/Stream.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Vec3s.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>


class BinaryReader final
{
public:
	explicit BinaryReader(Stream* stream);
	explicit BinaryReader(std::unique_ptr<Stream>&& stream);

	void Close();

	void Seek(std::uint32_t offset, SeekOffsetType seekType);
	[[nodiscard]] std::size_t GetBaseAddress();

	void Read(std::byte* buffer, std::size_t length);
	[[nodiscard]] char ReadChar();
	[[nodiscard]] std::int8_t ReadByte();
	[[nodiscard]] std::int16_t ReadInt16();
	[[nodiscard]] std::int32_t ReadInt32();
	[[nodiscard]] std::uint8_t ReadUByte();
	[[nodiscard]] std::uint16_t ReadUInt16();
	[[nodiscard]] std::uint32_t ReadUInt32();
	[[nodiscard]] std::uint64_t ReadUInt64();
	[[nodiscard]] float ReadSingle();
	[[nodiscard]] double ReadDouble();
	[[nodiscard]] std::string ReadString();

protected:
	std::unique_ptr<Stream> stream;
};