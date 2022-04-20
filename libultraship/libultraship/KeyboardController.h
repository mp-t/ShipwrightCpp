#pragma once
#include "Controller.h"
#include <string>

namespace Ship {
	class KeyboardController : public Controller {
		public:
			KeyboardController(std::int32_t dwControllerNumber);
			~KeyboardController();

			void ReadFromSource();
			void WriteToSource(ControllerCallback* controller);

			bool PressButton(std::int32_t dwScancode);
			bool ReleaseButton(std::int32_t dwScancode);
			void ReleaseAllButtons();

		protected:
			std::string GetControllerType();
			std::string GetConfSection();
			std::string GetBindingConfSection();
	};
}
