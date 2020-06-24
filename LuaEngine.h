#pragma once
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include <SDL2/SDL.h>
#undef main
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

void InitEngine();
void InitLuaEngine(lua_State* L);
lua_State* CreateLuaEngine();
void CloseEngine();
