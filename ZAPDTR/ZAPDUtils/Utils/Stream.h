#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

enum class SeekOffsetType
{
	Start,
	Current,
	End
};

// TODO: Eventually account for endianess in binaryreader and binarywriter
enum class Endianess
{
	Little = 0,
	Big = 1,
};

class Stream
{
public:
	virtual ~Stream() = default;
	virtual std::size_t GetLength() = 0;
	[[nodiscard]] std::size_t GetBaseAddress() { return baseAddress; }

	virtual void Seek(std::int32_t offset, SeekOffsetType seekType) = 0;

	virtual std::unique_ptr<std::byte[]> Read(std::size_t length) = 0;
	virtual void Read(std::byte* dest, std::size_t length) = 0;
	virtual std::byte ReadByte() = 0;

	virtual void Write(const std::byte* source, std::size_t length) = 0;
	virtual void WriteByte(std::byte value) = 0;

	virtual void Flush() = 0;
	virtual void Close() = 0;

protected:
	std::size_t baseAddress = 0;
};