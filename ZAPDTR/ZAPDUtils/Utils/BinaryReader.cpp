#include "Utils/BinaryReader.h"
#include "Utils/Stream.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

BinaryReader::BinaryReader(Stream* const stream) : stream(stream)
{
}

BinaryReader::BinaryReader(std::unique_ptr<Stream>&& stream) : stream(std::move(stream))
{
}

void BinaryReader::Close()
{
	stream->Close();
}

void BinaryReader::Seek(const std::uint32_t offset, const SeekOffsetType seekType)
{
	stream->Seek(offset, seekType);
}

std::size_t BinaryReader::GetBaseAddress()
{
	return stream->GetBaseAddress();
}

void BinaryReader::Read(std::byte* const buffer, const std::size_t length)
{
	stream->Read(buffer, length);
}

char BinaryReader::ReadChar()
{
	return static_cast<char>(stream->ReadByte());
}

std::int8_t BinaryReader::ReadByte()
{
	return static_cast<std::int8_t>(stream->ReadByte());
}

std::uint8_t BinaryReader::ReadUByte()
{
	return static_cast<std::uint8_t>(stream->ReadByte());
}

std::int16_t BinaryReader::ReadInt16()
{
	std::int16_t result = 0;

	stream->Read(reinterpret_cast<std::byte*>(&result), sizeof(std::int16_t));
	return result;
}

std::int32_t BinaryReader::ReadInt32()
{
	std::int32_t result = 0;

	stream->Read(reinterpret_cast<std::byte*>(&result), sizeof(std::int32_t));
	return result;
}

std::uint16_t BinaryReader::ReadUInt16()
{
	std::uint16_t result = 0;

	stream->Read(reinterpret_cast<std::byte*>(&result), sizeof(std::uint16_t));
	return result;
}

std::uint32_t BinaryReader::ReadUInt32()
{
	std::uint32_t result = 0;

	stream->Read(reinterpret_cast<std::byte*>(&result), sizeof(std::uint32_t));
	return result;
}

std::uint64_t BinaryReader::ReadUInt64()
{
	std::uint64_t result = 0;

	stream->Read(reinterpret_cast<std::byte*>(&result), sizeof(std::uint64_t));
	return result;
}

float BinaryReader::ReadSingle()
{
	float result = std::numeric_limits<float>::quiet_NaN();

	stream->Read(reinterpret_cast<std::byte*>(&result), sizeof(float));

	if (std::isnan(result))
	{
		throw std::runtime_error("BinaryReader::ReadSingle(): Error reading stream");
	}

	return result;
}

double BinaryReader::ReadDouble()
{
	double result = std::numeric_limits<double>::quiet_NaN();

	stream->Read(reinterpret_cast<std::byte*>(&result), sizeof(double));
	if (std::isnan(result))
	{
		throw std::runtime_error("BinaryReader::ReadDouble(): Error reading stream");
	}

	return result;
}

std::string BinaryReader::ReadString()
{
	std::string res;
	auto numChars = ReadInt32();

	while (numChars-->0)
	{
		res += ReadChar();
	}

	return res;
}