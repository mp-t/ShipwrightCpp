#pragma once

#include "Resource.h"
#include "Vertex.h"

#include <cstdint>

namespace Ship
{
	union ScalarData
	{
		std::uint8_t u8;
		std::int8_t s8;
		std::uint16_t u16;
		std::int16_t s16;
		std::uint32_t u32;
		std::int32_t s32;
		std::uint64_t u64;
		std::int64_t s64;
		float f32;
		double f64;
	};

	enum class ScalarType
	{
		ZSCALAR_NONE,
		ZSCALAR_S8,
		ZSCALAR_U8,
		ZSCALAR_X8,
		ZSCALAR_S16,
		ZSCALAR_U16,
		ZSCALAR_X16,
		ZSCALAR_S32,
		ZSCALAR_U32,
		ZSCALAR_X32,
		ZSCALAR_S64,
		ZSCALAR_U64,
		ZSCALAR_X64,
		ZSCALAR_F32,
		ZSCALAR_F64
	};

	// OTRTODO: Replace this with something that can be shared between the exporter and importer...
	enum class ZResourceType
	{
		Error,
		Animation,
		Array,
		AltHeader,
		Background,
		Blob,
		CollisionHeader,
		Cutscene,
		DisplayList,
		Limb,
		LimbTable,
		Mtx,
		Path,
		PlayerAnimationData,
		Room,
		RoomCommand,
		Scalar,
		Scene,
		Skeleton,
		String,
		Symbol,
		Texture,
		TextureAnimation,
		TextureAnimationParams,
		Vector,
		Vertex,
	};

	class ArrayV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class Array : public Resource
	{
	public:
		std::vector<ScalarData> scalars;
		std::vector<Vtx> vertices;

	};
}