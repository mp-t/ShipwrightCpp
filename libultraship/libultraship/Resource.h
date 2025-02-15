#pragma once

#include <stdint.h>
#include "Utils/BinaryReader.h"
#include "GlobalCtx2.h"
#include "StrHash.h"
#include "File.h"
#include "lib/tinyxml2/tinyxml2.h"

namespace Ship
{
    enum class ResourceType
    {
        Archive          =   0x4F415243,     // OARC
        Model            =   0x4F4D444C,     // OMDL
        Texture          =   0x4F544558,     // OTEX
        Material         =   0x4F4D4154,     // OMAT
        Animation        =   0x4F414E4D,     // OANM
        PlayerAnimation  =   0x4F50414D,     // OPAM
        DisplayList      =   0x4F444C54,     // ODLT
        Room             =   0x4F524F4D,     // OROM
        CollisionHeader  =   0x4F434F4C,     // OCOL
        Skeleton         =   0x4F534B4C,     // OSKL
        SkeletonLimb     =   0x4F534C42,     // OSLB
        Matrix           =   0x4F4D5458,     // OMTX
        Path             =   0x4F505448,     // OPTH
        Vertex           =   0x4F565458,     // OVTX
        Cutscene         =   0x4F435654,     // OCUT
        Array            =   0x4F415252,     // OARR
        Text             =   0x4F545854,     // OTXT
        Blob             =   0x4F424C42,     // OBLB
    };

    enum class DataType
    {
        U8 = 0,
        S8 = 1,
        U16 = 2,
        S16 = 3,
        U32 = 4,
        S32 = 5,
        U64 = 6,
        S64 = 7,
        F16 = 8,
        F32 = 9,
        F64 = 10
    };

    enum class Endianess
    {
        Little = 0,
        Big = 1,
    };

    enum class Version
    {
        // BR
        Deckard     = 0,
        Roy         = 1,
        Rachael     = 2,
        Leon        = 3,
        Zhora       = 4,
        // ...
    };

    struct Patch
    {
        std::uint64_t crc;
        std::uint32_t index;
        uintptr_t origData;
    };

    class Resource
    {
    public:
        ResourceMgr* resMgr;
        std::uint64_t id; // Unique Resource ID
        ResourceType resType;
        bool isDirty = false;
        void* cachedGameAsset = 0; // Conversion to OoT friendly struct cached...
        std::shared_ptr<File> file;
        std::vector<Patch> patches;
        virtual ~Resource();
    };

    class ResourceFile
    {
    public:
        Endianess endianess;    // 0x00 - Endianess of the file
        std::uint32_t resourceType;  // 0x01 - 4 byte MAGIC
        Version version;     // 0x05 - Based on Ship release numbers
        std::uint64_t id;            // 0x09 - Unique Resource ID
        std::uint32_t resourceVersion; // 0x11 - Resource Minor Version Number

        virtual void ParseFileBinary(BinaryReader* reader, Resource* res);
        virtual void ParseFileXML(tinyxml2::XMLElement* reader, Resource* res);
        virtual void WriteFileXML(tinyxml2::XMLElement* writer, Resource* res);
    };

    class ResourcePromise {
    public:
        std::shared_ptr<Resource> Resource;
        std::shared_ptr<File> File;
        std::condition_variable ResourceLoadNotifier;
        std::mutex ResourceLoadMutex;
        bool bHasResourceLoaded = false;
    };
}