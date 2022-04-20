#pragma once

#include "Resource.h"
#include <vector>
#include <string>

namespace Ship
{
	enum class SceneCommandID : std::uint8_t
	{
		SetStartPositionList = 0x00,
		SetActorList = 0x01,
		SetCsCamera = 0x02,
		SetCollisionHeader = 0x03,
		SetRoomList = 0x04,
		SetWind = 0x05,
		SetEntranceList = 0x06,
		SetSpecialObjects = 0x07,
		SetRoomBehavior = 0x08,
		Unused09 = 0x09,
		SetMesh = 0x0A,
		SetObjectList = 0x0B,
		SetLightList = 0x0C,
		SetPathways = 0x0D,
		SetTransitionActorList = 0x0E,
		SetLightingSettings = 0x0F,
		SetTimeSettings = 0x10,
		SetSkyboxSettings = 0x11,
		SetSkyboxModifier = 0x12,
		SetExitList = 0x13,
		EndMarker = 0x14,
		SetSoundSettings = 0x15,
		SetEchoSettings = 0x16,
		SetCutscenes = 0x17,
		SetAlternateHeaders = 0x18,
		SetCameraSettings = 0x19,

		// MM Commands
		SetWorldMapVisited = 0x19,
		SetAnimatedMaterialList = 0x1A,
		SetActorCutsceneList = 0x1B,
		SetMinimapList = 0x1C,
		Unused1D = 0x1D,
		SetMinimapChests = 0x1E,

		Error = 0xFF
	};

	class SceneCommand
	{
	public:
		SceneCommandID cmdID;
		void* cachedGameData = 0;

		// Data for a given class goes here...
		SceneCommand(BinaryReader* reader);
	};

	class LightingSettings
	{
	public:
		std::uint8_t ambientClrR, ambientClrG, ambientClrB;
		std::uint8_t diffuseDirA_X, diffuseDirA_Y, diffuseDirA_Z;
		std::uint8_t diffuseClrA_R, diffuseClrA_G, diffuseClrA_B;
		std::uint8_t diffuseDirB_X, diffuseDirB_Y, diffuseDirB_Z;
		std::uint8_t diffuseClrB_R, diffuseClrB_G, diffuseClrB_B;
		std::uint8_t fogClrR, fogClrG, fogClrB;
		std::uint16_t fogNear;
		std::uint16_t fogFar;
	};

	class LightInfo
	{
	public:
		LightInfo();
	public:
		std::uint8_t type;
		std::int16_t x, y, z;
		std::uint8_t r, g, b;
		std::uint8_t drawGlow;
		std::int16_t radius;
	};


	class SetWind : public SceneCommand
	{
	public:
		std::uint8_t windWest;
		std::uint8_t windVertical;
		std::uint8_t windSouth;
		std::uint8_t clothFlappingStrength;

		SetWind(BinaryReader* reader);
	};

	class ExitList : public SceneCommand
	{
	public:
		std::vector<std::uint16_t> exits;

		ExitList(BinaryReader* reader);
	};

	class SetTimeSettings : public SceneCommand
	{
	public:
		std::uint8_t hour;
		std::uint8_t min;
		std::uint8_t unk;

		SetTimeSettings(BinaryReader* reader);
	};

	class SetSkyboxModifier : public SceneCommand
	{
	public:
		std::uint8_t disableSky;
		std::uint8_t disableSunMoon;

		SetSkyboxModifier(BinaryReader* reader);
	};

	class SetEchoSettings : public SceneCommand
	{
	public:
		std::uint8_t echo;

		SetEchoSettings(BinaryReader* reader);
	};

	class SetSoundSettings : public SceneCommand
	{
	public:
		std::uint8_t reverb;
		std::uint8_t nightTimeSFX;
		std::uint8_t musicSequence;

		SetSoundSettings(BinaryReader* reader);
	};

	class SetSkyboxSettings : public SceneCommand
	{
	public:
		std::uint8_t unk1;  // (MM Only)
		std::uint8_t skyboxNumber;
		std::uint8_t cloudsType;
		std::uint8_t isIndoors;

		SetSkyboxSettings(BinaryReader* reader);
	};

	class SetRoomBehavior : public SceneCommand
	{
	public:
		std::uint8_t gameplayFlags;
		std::uint32_t gameplayFlags2;

		std::uint8_t currRoomUnk2;

		std::uint8_t showInvisActors;
		std::uint8_t currRoomUnk5;

		std::uint8_t msgCtxUnk;

		std::uint8_t enablePosLights;
		std::uint8_t kankyoContextUnkE2;

		SetRoomBehavior(BinaryReader* reader);
	};

	class SetCsCamera : public SceneCommand
	{
	public:
		SetCsCamera(BinaryReader* reader);
	};

	class BGImage
	{
	public:
		std::uint16_t unk_00;
		std::uint8_t id;
		std::string sourceBackground;
		std::uint32_t unk_0C;
		std::uint32_t tlut;
		std::uint16_t width;
		std::uint16_t height;
		std::uint8_t fmt;
		std::uint8_t siz;
		std::uint16_t mode0;
		std::uint16_t tlutCount;
	};

	class MeshData
	{
	public:
		int x, y, z;
		int unk_06;
		std::string opa, xlu;

		// Do I look like I know what a JPEG is?
		std::uint8_t imgFmt;
		std::string imgOpa, imgXlu;
		std::vector<BGImage> images;

		MeshData();
	};

	class SetMesh : public SceneCommand
	{
	public:
		std::uint8_t data;
		std::uint8_t meshHeaderType;
		std::vector<MeshData> meshes;
		//std::shared_ptr<PolygonTypeBase> polyType;

		SetMesh(BinaryReader* reader);
	};

	class SetCameraSettings : public SceneCommand
	{
	public:
		std::uint8_t cameraMovement;
		std::uint32_t mapHighlights;

		SetCameraSettings(BinaryReader* reader);
	};

	class SetLightingSettings : public SceneCommand
	{
	public:
		std::vector<LightingSettings> settings;

		SetLightingSettings(BinaryReader* reader);
	};

	class SetLightList : public SceneCommand
	{
	public:
		std::vector<LightInfo> lights;

		SetLightList(BinaryReader* reader);
	};

	class SetRoom
	{
	public:
		std::string name;
		std::uint32_t vromStart;
		std::uint32_t vromEnd;

		SetRoom(BinaryReader* reader);
	};

	class SetRoomList : public SceneCommand
	{
	public:
		std::vector<SetRoom> rooms;
		
		SetRoomList(BinaryReader* reader);
	};

	class SetCollisionHeader : public SceneCommand
	{
	public:
		std::string filePath;

		SetCollisionHeader(BinaryReader* reader);
	};

	class EntranceEntry
	{
	public:
		std::uint8_t startPositionIndex;
		std::uint8_t roomToLoad;
	};

	class SetEntranceList : public SceneCommand
	{
	public:
		std::vector<EntranceEntry> entrances;

		SetEntranceList(BinaryReader* reader);
	};

	class SetSpecialObjects : public SceneCommand
	{
	public:
		std::uint8_t elfMessage;
		std::uint16_t globalObject;
		
		SetSpecialObjects(BinaryReader* reader);
	};

	class SetObjectList : public SceneCommand
	{
	public:
		std::vector<std::uint16_t> objects;

		SetObjectList(BinaryReader* reader);
	};
	
	class SetCutscenes : public SceneCommand
	{
	public:
		std::string cutscenePath;

		SetCutscenes(BinaryReader* reader);
	};

	class SetPathways : public SceneCommand
	{
	public:
		std::vector<std::string> paths;

		SetPathways(BinaryReader* reader);
	};

	class SetAlternateHeaders : public SceneCommand
	{
	public:
		std::vector<std::string> headers;

		SetAlternateHeaders(BinaryReader* reader);
	};

	class TransitionActorEntry
	{
	public:
		std::uint8_t frontObjectRoom;
		std::uint8_t frontTransitionReaction;
		std::uint8_t backObjectRoom;
		std::uint8_t backTransitionReaction;
		std::uint16_t actorNum;
		std::int16_t posX, posY, posZ;
		std::int16_t rotY;
		std::uint16_t initVar;
	};

	class ActorSpawnEntry
	{
	public:
		std::uint16_t actorNum;
		std::int16_t posX;
		std::int16_t posY;
		std::int16_t posZ;
		std::int16_t rotX;
		std::int16_t rotY;
		std::int16_t rotZ;
		std::uint16_t initVar;
	};


	class SetStartPositionList : public SceneCommand
	{
	public:
		std::vector<ActorSpawnEntry> entries;

		SetStartPositionList(BinaryReader* reader);
	};

	class SetActorList : public SceneCommand
	{
	public:
		std::vector<ActorSpawnEntry> entries;

		SetActorList(BinaryReader* reader);
	};

	class SetTransitionActorList : public SceneCommand
	{
	public:
		std::vector<TransitionActorEntry> entries;

		SetTransitionActorList(BinaryReader* reader);
	};


	class EndMarker : public SceneCommand
	{
	public:
		EndMarker(BinaryReader* reader);
	};

	class Scene : public Resource
	{
	public:
		std::vector<SceneCommand*> commands;

		~Scene();
	};

	class SceneV0 : public ResourceFile
	{
	public:
		SceneCommand* ParseSceneCommand(BinaryReader* reader);
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};
}