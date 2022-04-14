#include "CollisionHeader.h"

class BinaryReader;

namespace Ship::CollisionHeaderFactory
{

[[nodiscard]] CollisionHeader* ReadCollisionHeader(BinaryReader* reader);

}