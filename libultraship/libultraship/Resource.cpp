#include "Resource.h"
#include "DisplayList.h"
#include "ResourceMgr.h"
#include "Utils/BinaryReader.h"
#include "lib/tinyxml2/tinyxml2.h"
#include "lib/Fast3D/U64/PR/ultra64/gbi.h"

namespace Ship
{
    void ResourceFile::ParseFileBinary(BinaryReader* reader, Resource* res)
    {
        id = reader->ReadUInt64();
        res->id = id;
        [[maybe_unused]] const auto minorVersion = reader->ReadUInt32(); // Resource minor version number
        [[maybe_unused]] const auto crc = reader->ReadUInt64(); // ROM CRC
        [[maybe_unused]] const auto romEnum = reader->ReadUInt32(); // ROM Enum

        // Reserved for future file format versions...
        reader->Seek(64, SeekOffsetType::Start);
    }
    void ResourceFile::ParseFileXML(tinyxml2::XMLElement* reader, Resource*)
    {
        id = reader->Int64Attribute("id", -1);
    }

    void ResourceFile::WriteFileXML(tinyxml2::XMLElement*, Resource*)
    {

    }

    Resource::~Resource()
    {
        free(cachedGameAsset); 
        cachedGameAsset = nullptr;

        for (std::size_t i = 0; i < patches.size(); i++)
        {
            std::string hashStr = resMgr->HashToString(patches[i].crc);
            auto resShared = resMgr->GetCachedFile(hashStr);
            if (resShared != nullptr)
            {
                auto res = (Ship::DisplayList*)resShared.get();
                
                Gfx* gfx = (Gfx*)&res->instructions[patches[i].index];
                gfx->words.w1 = patches[i].origData;
            }
        }

        patches.clear();

#if _DEBUG
        if (file != nullptr)
            printf("Deconstructor called on file %s\n", file->path.c_str());
#endif
    }
}