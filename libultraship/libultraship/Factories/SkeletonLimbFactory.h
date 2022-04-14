#include "SkeletonLimb.h"

class BinaryReader;

namespace Ship::SkeletonLimbFactory
{

[[nodiscard]] SkeletonLimb* ReadSkeletonLimb(BinaryReader* reader);

}