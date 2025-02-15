#include "SohHooks.h"
#include <map>
#include <string>
#include <vector>
#include <stdarg.h>
#include <iostream>

std::map<std::string, std::vector<HookFunc>> listeners;
std::string hookName;
std::map<std::string, const void*> initArgs;
std::map<std::string, const void*> hookArgs;

/*
#############################
   Module: Hook C++ Handle
#############################
*/

namespace ModInternal {

    void registerHookListener(HookListener listener) {
        listeners[listener.hookName].push_back(listener.callback);
    }

    bool handleHook(std::shared_ptr<HookCall> call) {
        std::string name = std::string(call->name);
        for (std::size_t l = 0; l < listeners[name].size(); l++) {
            (listeners[name][l])(call);
        }
        return call->cancelled;
    }

    void bindHook(std::string name) {
        hookName = name;
    }

    void initBindHook(int length, ...) {
        if (length > 0) {
            va_list args;
            va_start(args, length);
            for (int i = 0; i < length; i++) {
                HookParameter currentParam = va_arg(args, struct HookParameter);
                initArgs[currentParam.name] = currentParam.parameter;
            }
            va_end(args);
        }
    }

    bool callBindHook(int length, ...) {
        if (length > 0) {
            va_list args;
            va_start(args, length);
            for (int i = 0; i < length; i++) {
                HookParameter currentParam = va_arg(args, struct HookParameter);
                hookArgs[currentParam.name] = currentParam.parameter;
            }
            va_end(args);
        }

        HookCall call = {
            .name = hookName,
            .baseArgs = initArgs,
            .hookedArgs = hookArgs
		};
        const bool cancelled = handleHook(std::make_shared<HookCall>(call));

        hookName = "";
        initArgs.clear();
        hookArgs.clear();

        return cancelled;
    }
}

/*
#############################
    Module: Hook C Handle
#############################
*/

void bind_hook(const char* const name) {
	hookName = std::string(name);
}

void init_hook(int length, ...) {
	if (length > 0) {
		va_list args;
		va_start(args, length);
		for (int i = 0; i < length; i++) {
			HookParameter currentParam = va_arg(args, struct HookParameter);
			initArgs[currentParam.name] = currentParam.parameter;
		}
		va_end(args);
	}
}

bool call_hook(int length, ...) {
	if (length > 0) {
		va_list args;
		va_start(args, length);
		for (int i = 0; i < length; i++) {
			HookParameter currentParam = va_arg(args, struct HookParameter);
			hookArgs[currentParam.name] = currentParam.parameter;
		}
		va_end(args);
	}

	HookCall call = {
		.name = hookName,
		.baseArgs = initArgs,
		.hookedArgs = hookArgs
	};

	const bool cancelled = ModInternal::handleHook(std::make_shared<HookCall>(call));

	hookName = "";
	initArgs.clear();
	hookArgs.clear();

	return cancelled;
}
