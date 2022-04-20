#include "KeyboardController.h"
#include "GlobalCtx2.h"

namespace Ship {
	KeyboardController::KeyboardController(std::int32_t dwControllerNumber) : Controller(dwControllerNumber) {
		LoadBinding();
	}

	KeyboardController::~KeyboardController() {
		
	}

	bool KeyboardController::PressButton(std::int32_t dwScancode) {
		if (ButtonMapping.contains(dwScancode)) {
			dwPressedButtons |= ButtonMapping[dwScancode];
			return true;
		}

		return false;
	}

	bool KeyboardController::ReleaseButton(std::int32_t dwScancode) {
		if (ButtonMapping.contains(dwScancode)) {
			dwPressedButtons &= ~ButtonMapping[dwScancode];
			return true;
		}

		return false;
	}

	void KeyboardController::ReleaseAllButtons() {
		dwPressedButtons = 0;
	}

	void KeyboardController::ReadFromSource() {
		wStickX = 0;
		wStickY = 0;
	}

	void KeyboardController::WriteToSource(ControllerCallback*)
	{

	}

	std::string KeyboardController::GetControllerType() {
		return "KEYBOARD";
	}

	std::string KeyboardController::GetConfSection() {
		return GetControllerType() + " CONTROLLER " + std::to_string(GetControllerNumber() + 1);
	}

	std::string KeyboardController::GetBindingConfSection() {
		return GetControllerType() + " CONTROLLER BINDING " + std::to_string(GetControllerNumber() + 1);
	}
}