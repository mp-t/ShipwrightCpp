#pragma once

#include "Blob.h"

class BinaryReader;

namespace Ship::BlobFactory
{

[[nodiscard]] Blob* ReadBlob(BinaryReader* reader);

}