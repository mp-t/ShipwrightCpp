#include "cvar.h"
#include <map>
#include <string>
#include <PR/ultra64/gbi.h>

std::map<std::string, CVar*> cvars;

CVar* CVar_GetVar(const char* const name) {
    std::string key(name);
    return cvars.contains(key) ? cvars[key] : nullptr;
}

CVar* CVar_Get(const char* const name) {
    return CVar_GetVar(name);
}

s32 CVar_GetS32(const char* const name, const s32 defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVAR_TYPE_S32)
            return cvar->value.valueS32;
    }

    return defaultValue;
}

float CVar_GetFloat(const char* const name, const float defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVAR_TYPE_FLOAT)
            return cvar->value.valueFloat;
    }

    return defaultValue;
}

char* CVar_GetString(const char* const name, char* const defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVAR_TYPE_STRING)
            return cvar->value.valueStr;
    }

    return defaultValue;
}

void CVar_SetS32(const char* const name, const s32 value) {
    CVar* cvar = CVar_Get(name);
    if (!cvar) {
        cvar = new CVar;
        cvars[std::string(name)] = cvar;
    }
    cvar->type = CVAR_TYPE_S32;
    cvar->value.valueS32 = value;
}

void CVar_SetFloat(const char* const name, const float value) {
    CVar* cvar = CVar_Get(name);
    if (!cvar) {
        cvar = new CVar;
        cvars[std::string(name)] = cvar;
    }
    cvar->type = CVAR_TYPE_FLOAT;
    cvar->value.valueFloat = value;
}

void CVar_SetString(const char* const name, char* const value) {
    CVar* cvar = CVar_Get(name);
    if (!cvar) {
        cvar = new CVar;
        cvars[std::string(name)] = cvar;
    }
    cvar->type = CVAR_TYPE_STRING;
    cvar->value.valueStr = value;
}


void CVar_RegisterS32(const char* const name, const s32 defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar == nullptr)
        CVar_SetS32(name, defaultValue);
}

void CVar_RegisterFloat(const char* const name, const float defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar == nullptr)
        CVar_SetFloat(name, defaultValue);
}

void CVar_RegisterString(const char* const name, char* const defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar == nullptr)
        CVar_SetString(name, defaultValue);
}