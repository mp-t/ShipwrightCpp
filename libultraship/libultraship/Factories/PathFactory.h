#include "Path.h"

class BinaryReader;

namespace Ship::PathFactory
{

[[nodiscard]] Path* ReadPath(BinaryReader* reader);

}