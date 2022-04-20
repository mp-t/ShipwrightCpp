#include "Texture.h"

namespace Ship
{
    void TextureV0::ParseFileBinary(BinaryReader* reader, Resource* res)
    {
        Texture* tex = (Texture*)res;

        ResourceFile::ParseFileBinary(reader, tex);

        tex->texType = (TextureType)reader->ReadUInt32();
        tex->width = static_cast<std::uint16_t>(reader->ReadUInt32());
        tex->height = static_cast<std::uint16_t>(reader->ReadUInt32());

        std::uint32_t dataSize = reader->ReadUInt32();

        tex->imageDataSize = dataSize;
        tex->imageData = new std::uint8_t[dataSize];

        for (std::uint32_t i = 0; i < dataSize; i++)
            tex->imageData[i] = reader->ReadUByte();
    }
}