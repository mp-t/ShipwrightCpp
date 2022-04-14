#pragma once

#include "GlobalCtx2.h"

#include <cstddef>
#include <memory>
#include <string>

namespace Ship {
	class Archive;

	class File
	{
	public:
		std::shared_ptr<Archive> parent;
		std::string path;
		std::shared_ptr<std::byte[]> buffer;
		uint32_t dwBufferSize;
		bool bIsLoaded = false;
		bool bHasLoadError = false;
		std::condition_variable FileLoadNotifier;
		std::mutex FileLoadMutex;
	};
}
