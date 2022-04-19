#include "Utils/MemoryStream.h"

#include <cstring>

MemoryStream::MemoryStream(const std::byte* const nBuffer, const std::size_t bufferSize)
{
	buffer.assign(nBuffer, nBuffer + bufferSize);
}

std::size_t MemoryStream::GetLength()
{
	return buffer.size();
}

void MemoryStream::Seek(const std::int32_t offset, const SeekOffsetType seekType)
{
	if (seekType == SeekOffsetType::Start)
	{
		baseAddress = offset;
	}
	else if (seekType == SeekOffsetType::Current)
	{
		baseAddress += offset;
	}
	else if (seekType == SeekOffsetType::End)
	{
		baseAddress = buffer.size() - 1 - offset;
	}
}

std::unique_ptr<std::byte[]> MemoryStream::Read(const std::size_t length)
{
	auto result = std::make_unique<std::byte[]>(length);

	memcpy_s(result.get(), length, &buffer[baseAddress], length);
	baseAddress += length;

	return result;
}

void MemoryStream::Read(std::byte* const dest, const std::size_t length)
{
	memcpy_s(dest, length, &buffer[baseAddress], length);
	baseAddress += length;
}

std::byte MemoryStream::ReadByte()
{
	return buffer[baseAddress++];
}

void MemoryStream::Write(const std::byte* const srcBuffer, const std::size_t length)
{
	if (baseAddress + length >= buffer.size())
	{
		buffer.resize(baseAddress + length);
	}

	memcpy_s(&buffer[baseAddress], buffer.size() - baseAddress, srcBuffer, length);
	baseAddress += length;
}

void MemoryStream::WriteByte(const std::byte value)
{
	if (baseAddress + 1 >= buffer.size())
	{
		buffer.resize(baseAddress + 1);
	}

	buffer[baseAddress++] = value;
}

std::vector<std::byte> MemoryStream::ToVector()
{
	return buffer;
}

void MemoryStream::Flush()
{
}

void MemoryStream::Close()
{
}