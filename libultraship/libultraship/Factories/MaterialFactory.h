#include "Material.h"

class BinaryReader;

namespace Ship::MaterialFactory
{

[[nodiscard]] Material* ReadMaterial(BinaryReader* reader);

}