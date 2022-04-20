#pragma once

#include "DisplayList.h"

class BinaryReader;

namespace Ship::DisplayListFactory
{

[[nodiscard]] DisplayList* ReadDisplayList(BinaryReader* reader);

}