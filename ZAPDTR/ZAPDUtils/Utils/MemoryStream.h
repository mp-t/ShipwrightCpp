#pragma once

#include "Utils/Stream.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

class MemoryStream : public Stream
{
public:
	MemoryStream() = default;
	MemoryStream(const std::byte* buffer, std::size_t bufferSize);
	~MemoryStream() = default;

	[[nodiscard]] std::size_t GetLength() override;

	void Seek(std::int32_t offset, SeekOffsetType seekType) override;

	[[nodiscard]] std::unique_ptr<std::byte[]> Read(std::size_t length) override;
	void Read(std::byte* dest, std::size_t length) override;
	[[nodiscard]] std::byte ReadByte() override;

	void Write(const std::byte* srcBuffer, std::size_t length) override;
	void WriteByte(std::byte value) override;

	[[nodiscard]] std::vector<std::byte> ToVector();

	void Flush() override;
	void Close() override;

protected:
	std::vector<std::byte> buffer;
};