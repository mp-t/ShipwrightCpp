#include "Skeleton.h"

class BinaryReader;

namespace Ship::SkeletonFactory
{

[[nodiscard]] Skeleton* ReadSkeleton(BinaryReader* reader);

}