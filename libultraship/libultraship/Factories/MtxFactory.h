#pragma once

#include "Matrix.h"

class BinaryReader;

namespace Ship::MtxFactory
{

[[nodiscard]] Matrix* ReadMtx(BinaryReader* reader);

}