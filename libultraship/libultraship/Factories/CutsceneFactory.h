#pragma once

#include "Cutscene.h"

class BinaryReader;

namespace Ship::CutsceneFactory
{

[[nodiscard]] Cutscene* ReadCutscene(BinaryReader* reader);

}