#pragma once
#include "Vertex.h"

class BinaryReader;

namespace Ship::VertexFactory
{
	
[[nodiscard]] Vertex* ReadVtx(BinaryReader* reader);

}