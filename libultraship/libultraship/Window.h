#pragma once
#include <memory>
#include "PR/ultra64/gbi.h"
#include "Lib/Fast3D/gfx_pc.h"
#include "UltraController.h"
#include "Controller.h"
#include "GlobalCtx2.h"

namespace Ship {
	class AudioPlayer;

	class Window {
		public:
			static std::map<size_t, std::vector<std::shared_ptr<Controller>>> Controllers;
			static std::int32_t lastScancode;

			Window(std::shared_ptr<GlobalCtx2> Context);
			~Window();
			void MainLoop(void (*MainFunction)(void));
			void Init();
			void RunCommands(Gfx* Commands);
			void SetFrameDivisor(int divisor);
			std::uint16_t GetPixelDepth(float x, float y);
			void ToggleFullscreen();
			void SetFullscreen(bool bIsFullscreen);
			void ShowCursor(bool hide);

			bool IsFullscreen() { return bIsFullscreen; }
			std::uint32_t GetCurrentWidth();
			std::uint32_t GetCurrentHeight();
			std::uint32_t dwMenubar;
			std::shared_ptr<GlobalCtx2> GetContext() { return Context.lock(); }
			std::shared_ptr<AudioPlayer> GetAudioPlayer() { return APlayer; }

		protected:
		private:
			static bool KeyDown(std::int32_t dwScancode);
			static bool KeyUp(std::int32_t dwScancode);
			static void AllKeysUp(void);
			static void OnFullscreenChanged(bool bIsNowFullscreen);
			void SetAudioPlayer();

			std::weak_ptr<GlobalCtx2> Context;
			std::shared_ptr<AudioPlayer> APlayer;

			GfxWindowManagerAPI* WmApi;
			GfxRenderingAPI* RenderingApi;
			bool bIsFullscreen;
			std::uint32_t dwWidth;
			std::uint32_t dwHeight;
	};
}

