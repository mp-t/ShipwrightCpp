#include "Scene.h"

class BinaryReader;

namespace Ship::SceneFactory
{

[[nodiscard]] Scene* ReadScene(BinaryReader* reader);

}