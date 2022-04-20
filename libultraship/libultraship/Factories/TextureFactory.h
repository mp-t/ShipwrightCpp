#pragma once

#include "Texture.h"

class BinaryReader;

namespace Ship::TextureFactory
{

[[nodiscard]] Texture* ReadTexture(BinaryReader* reader);

}