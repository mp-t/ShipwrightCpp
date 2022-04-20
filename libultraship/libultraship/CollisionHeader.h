#pragma once

#include <vector>
#include <string>
#include "Resource.h"
#include "Vec2f.h"
#include "Vec3f.h"
#include "Color3b.h"

namespace Ship
{
	class PolygonEntry
	{
	public:
		std::uint16_t type;
		std::uint16_t vtxA, vtxB, vtxC;
		std::uint16_t a, b, c, d;

		PolygonEntry(BinaryReader* reader);
	};

	class WaterBoxHeader
	{
	public:
		std::int16_t xMin;
		std::int16_t ySurface;
		std::int16_t zMin;
		std::int16_t xLength;
		std::int16_t zLength;
		std::int16_t pad;
		std::int32_t properties;

		WaterBoxHeader();
	};


	class CameraDataEntry
	{
	public:
		std::uint16_t cameraSType;
		std::int16_t numData;
		std::int32_t cameraPosDataIdx;
	};

	class CameraPositionData
	{
	public:
		std::int16_t x, y, z;
	};

	class CameraDataList
	{
	public:
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