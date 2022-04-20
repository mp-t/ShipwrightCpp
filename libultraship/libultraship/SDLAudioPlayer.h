#pragma once
#include "AudioPlayer.h"

#include <SDL2/SDL.h>

#include <cstdint>

namespace Ship {
	class SDLAudioPlayer : public AudioPlayer {
	public:
		SDLAudioPlayer() {  };

		bool Init(void);
		int Buffered(void);
		int GetDesiredBuffered(void);
		void Play(const std::uint8_t* Buffer, std::uint32_t BufferLen);

	private:
		SDL_AudioDeviceID Device;
	};
}
