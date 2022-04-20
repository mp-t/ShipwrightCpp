#pragma once

#include <memory>
#include <map>
#include <string>
#include "stdint.h"
#include "UltraController.h"
#include "ControllerAttachment.h"

#define EXTENDED_SCANCODE_BIT (1 << 8)
#define AXIS_SCANCODE_BIT (1 << 9)

namespace Ship {
	class Controller {

		public:
			Controller(std::int32_t dwControllerNumber);

			void Read(OSContPad* pad);
			virtual void ReadFromSource() = 0;
			virtual void WriteToSource(ControllerCallback* controller) = 0;
			bool isRumbling;

			void SetButtonMapping(const std::string& szButtonName, std::int32_t dwScancode);
			std::shared_ptr<ControllerAttachment> GetAttachment() { return Attachment; }
			std::int32_t GetControllerNumber() { return dwControllerNumber; }

		protected:
			std::int32_t dwPressedButtons;
			std::map<std::int32_t, std::int32_t> ButtonMapping;
			std::int8_t wStickX;
			std::int8_t wStickY;
			float wGyroX;
			float wGyroY;
			
			virtual std::string GetControllerType() = 0;
			virtual std::string GetConfSection() = 0;
			virtual std::string GetBindingConfSection() = 0;
			void LoadBinding();

		private:
			std::shared_ptr<ControllerAttachment> Attachment;
			std::int32_t dwControllerNumber;
	};
}
