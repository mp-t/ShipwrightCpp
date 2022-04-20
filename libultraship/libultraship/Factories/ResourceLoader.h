#pragma once

#include "File.h"
#include "Resource.h"

#include <memory>

namespace Ship::ResourceLoader
{

[[nodiscard]] Resource* LoadResource(std::shared_ptr<File> FileToLoad);

}