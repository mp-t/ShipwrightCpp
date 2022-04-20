#pragma once

#include "Resource.h"

#include <Utils/BinaryReader.h>
#include <Vec3f.h>

#include <cstdint>
#include <vector>

namespace Ship
{

class PolygonEntry
{
public:
	std::uint16_t type;
	std::uint16_t vtxA, vtxB, vtxC;
	std::uint16_t a, b, c, d;

	explicit PolygonEntry(BinaryReader* reader);
};

struct WaterBoxHeader
{
	std::int16_t xMin;
	std::int16_t ySurface;
	std::int16_t zMin;
	std::int16_t xLength;
	std::int16_t zLength;
	std::int16_t pad;
	std::int32_t properties;
};


struct CameraDataEntry
{
	std::uint16_t cameraSType;
	std::int16_t numData;
	std::int32_t cameraPosDataIdx;
};

struct CameraPositionData
{
	std::int16_t x, y, z;
};

struct CameraDataList
{
	std::vector<CameraDataEntry*> entries;
	std::vector<CameraPositionData*> cameraPositionData;
};

class CollisionHeaderV0 : public ResourceFile
{
public:
	std::int16_t absMinX, absMinY, absMinZ;
	std::int16_t absMaxX, absMaxY, absMaxZ;

	std::vector<Vec3f> vertices;
	std::vector<PolygonEntry> polygons;
	std::vector<std::uint64_t> polygonTypes;
	std::vector<WaterBoxHeader> waterBoxes;
	CameraDataList* camData = nullptr;

	void ParseFileBinary(BinaryReader* reader, Resource* res) override;
};

class CollisionHeader : public Resource
{
public:
	std::int16_t absMinX, absMinY, absMinZ;
	std::int16_t absMaxX, absMaxY, absMaxZ;

	std::vector<Vec3f> vertices;
	std::vector<PolygonEntry> polygons;
	std::vector<std::uint64_t> polygonTypes;
	std::vector<WaterBoxHeader> waterBoxes;
	CameraDataList* camData = nullptr;
};

}