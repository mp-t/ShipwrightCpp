#pragma once
#include "Controller.h"
#include <SDL2/SDL.h>

#define INVALID_SDL_CONTROLLER_GUID (std::string("00000000000000000000000000000000"))

namespace Ship {
	class SDLController : public Controller {
		public:
			SDLController(std::int32_t dwControllerNumber);
			~SDLController();

			void ReadFromSource();
			void WriteToSource(ControllerCallback* controller);

			std::string GetGuid() { return guid; };

		protected:
			std::string GetControllerType();
			void SetButtonMapping(const std::string& szButtonName, std::int32_t dwScancode);
			std::string GetConfSection();
			std::string GetBindingConfSection();
			void CreateDefaultBinding();
			static bool IsGuidInUse(const std::string& guid);

		private:
			std::string guid;
			SDL_GameController* Cont;
			std::map<std::int32_t, std::int16_t> ThresholdMapping;

			void LoadAxisThresholds();
			void NormalizeStickAxis(std::int16_t wAxisValueX, std::int16_t wAxisValueY, std::int16_t wAxisThreshold);
			bool Open();
			bool Close();
	};
}
