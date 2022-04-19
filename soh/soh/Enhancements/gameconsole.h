#ifndef _GAMECONSOLE_H_
#define _GAMECONSOLE_H_

#include <ultra64.h>
#include <z64.h>
#include "cvar.h"

#define MAX_CVARS 2048

typedef enum
{
    CONSOLECOMMAND_RESULT_SUCCESS,
    CONSOLECOMMAND_RESULT_INVALIDARGS,
    CONSOLECOMMAND_RESULT_INVALIDCOMMAND,
    CONSOLECOMMAND_RESULT_FAILURE
} ConsoleCommandResult;

typedef ConsoleCommandResult(*ConsoleCommandFunc)(char** argv, s32 argc); // Returns the number of arguments it read

typedef struct ConsoleCommand
{
    char* name;
    ConsoleCommandFunc func;
    char* description;
} ConsoleCommand;

extern GlobalContext* gGlobalCtx;

void GameConsole_Init();

s32 GameConsole_Split(char* str, char** argv);

#endif