#pragma once

#include "Array.h"

class BinaryReader;

namespace Ship::ArrayFactory
{

[[nodiscard]] Array* ReadArray(BinaryReader* reader);

}