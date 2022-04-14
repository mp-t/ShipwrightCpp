#include "PlayerAnimation.h"

class BinaryReader;

namespace Ship::PlayerAnimationFactory
{

[[nodiscard]] PlayerAnimation* ReadPlayerAnimation(BinaryReader* reader);

}