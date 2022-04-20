#pragma once

#include "Animation.h"

class BinaryReader;

namespace Ship::AnimationFactory
{

[[nodiscard]] Animation* ReadAnimation(BinaryReader* reader);

}