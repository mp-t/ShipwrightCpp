#pragma once

#include <cstdint>

namespace Ship {
	class AudioPlayer {

	public:
		AudioPlayer() {  };

		virtual bool Init(void) = 0;
		virtual int Buffered(void) = 0;
		virtual int GetDesiredBuffered(void) = 0;
		virtual void Play(const std::uint8_t* buf, std::uint32_t len) = 0;
	};
}
