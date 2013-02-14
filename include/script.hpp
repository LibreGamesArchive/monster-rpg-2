#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "monster2.hpp"

class Object;

unsigned char *slurp_file(std::string filename, int *ret_size);

void openLuaLibs(lua_State* state);
void callLua(lua_State* luaState, const char *func, const char *sig, ...);
void dumpLuaStack(lua_State* stack);
void registerCFunctions(lua_State* luaState);
std::string getScriptExtension(void);
void startNewGame(const char *name = "start");
int getNumberFromScript(lua_State *state, std::string name);
void runGlobalScript(lua_State *luaState);
void setObjectDirection(Object *o, int direction);
bool anotherDoDialogue(const char *text, bool clearbuf = true, bool top = false);
int CDeScriptifyPlayer(lua_State *stack); // doesn't use stack

extern bool player_scripted;

extern bool shouldDoMap;
extern std::string mapStartPlace;
extern std::string mapPrefix;

#endif
