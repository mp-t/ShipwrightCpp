#include "Text.h"

class BinaryReader;

namespace Ship::TextFactory
{

[[nodiscard]] Text* ReadText(BinaryReader* reader);

}