#pragma once
#include "LuaEngine.h"

#define SDLError(L, prompt) luaL_error(L, #prompt ": %s", SDL_GetError())
#define TTFError(L, prompt) luaL_error(L, #prompt ": %s", TTF_GetError())
#define IMGError(L, prompt) luaL_error(L, #prompt ": %s", IMG_GetError())
#define MixError(L, prompt) luaL_error(L, #prompt ": %s", Mix_GetError())

#define lua_setfield_function(L, name, func) lua_pushcfunction(L, func);lua_setfield(L, -2, name)

template<typename PointerType>
void lua_newpointer(lua_State* L, PointerType* p)
{
	auto ptr = (PointerType**)lua_newuserdata(L, sizeof(PointerType*));
	*ptr = p;
}

template<typename PointerType>
PointerType* lua_checkpointer(lua_State* L, int idx, const char* name)
{
	return *(PointerType**)luaL_checkudata(L, idx, name);
}

template<typename T>
T* lua_newblock(lua_State* L)
{
	return (T*)lua_newuserdata(L, sizeof(T));
}

template<typename T>
T* lua_checkblock(lua_State* L, int idx, const char* name)
{
	return (T*)luaL_checkudata(L, idx, name);
}

// Shared Functions
void put_surface(lua_State* L, SDL_Surface* surf);
void put_texture(lua_State* L, SDL_Texture* text);

// Init Functions
void InitWindow(lua_State* L);
void InitRenderer(lua_State* L);
void InitFont(lua_State* L);
void InitMusic(lua_State* L);
void InitEvent(lua_State* L);
void InitCommon(lua_State* L);
void InitTCPSocket(lua_State* L);
void InitThread(lua_State* L);
void InitSocketSelector(lua_State* L);

void PlatInit();

#ifdef _WIN32
void put_winerror(lua_State* L, int errcode, const char* hint);

#endif  // ifdef _WIN32

int lua_errno(lua_State* L, const char* hint);
