#include "include.h"
using namespace std;

/**
module Surface
	load(filename: string): Surface
	loadmem(data: string): Surface

class Surface
	enableColorKey(r: int, g: int, b: int)
	disableColorKey(r: int, g: int, b: int)
	enableRLE()
	disableRLE()
*/

int surface_enablecolorkey(lua_State* L)
{
	auto surf = lua_checkres<SDL_Surface>(L, 1, "LuaEngineSurface");
	int r = luaL_checkinteger(L, 2);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 2);
	if (SDL_SetColorKey(surf, 1, SDL_MapRGB(surf->format, r, g, b)) != 0)
	{
		return SDLError(L, SDL_SetColorKey);
	}
	return 0;
}

int surface_disablecolorkey(lua_State* L)
{
	auto surf = lua_checkres<SDL_Surface>(L, 1, "LuaEngineSurface");
	int r = luaL_checkinteger(L, 2);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 2);
	if (SDL_SetColorKey(surf, 0, SDL_MapRGB(surf->format, r, g, b)) != 0)
	{
		return SDLError(L, SDL_SetColorKey);
	}
	return 0;
}

int surface_enableRLE(lua_State* L)
{
	auto surf = lua_checkres<SDL_Surface>(L, 1, "LuaEngineSurface");
	if (SDL_SetSurfaceRLE(surf, 1) != 0)
	{
		return SDLError(L, SDL_SetSurfaceRLE);
	}
	return 0;
}

int surface_disableRLE(lua_State* L)
{
	auto surf = lua_checkres<SDL_Surface>(L, 1, "LuaEngineSurface");
	if (SDL_SetSurfaceRLE(surf, 0) != 0)
	{
		return SDLError(L, SDL_SetSurfaceRLE);
	}
	return 0;
}

int surface_close(lua_State* L)
{
	auto surf = lua_checkfullres<SDL_Surface>(L, 1, "LuaEngineSurface");
	surf->~ShareableResource<SDL_Surface>();
	return 0;
}

// shared
void put_surface_meta(lua_State* L)
{
	if (luaL_newmetatable(L, "LuaEngineSurface"))
	{
		lua_setfield_function(L, "__gc", surface_close);
		lua_newtable(L);
		lua_setfield_function(L, "enableColorKey", surface_enablecolorkey);
		lua_setfield_function(L, "disableColorKey", surface_disablecolorkey);
		lua_setfield_function(L, "enableRLE", surface_enableRLE);
		lua_setfield_function(L, "disableRLE", surface_disableRLE);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

// global
void put_surface(lua_State* L, SDL_Surface* surf)
{
	lua_newres(L, surf, SDL_FreeSurface);
	put_surface_meta(L);
}

// global
void put_surface(lua_State* L, const shared_ptr<SDL_Surface>& surf)
{
	lua_newres(L, surf);
	put_surface_meta(L);
}

int surface_load(lua_State* L)
{
	const char* filename = luaL_checkstring(L, 1);
	SDL_Surface* surf = IMG_Load(filename);
	if (!surf)
	{
		return IMGError(L, IMG_Load);
	}
	put_surface(L, surf);
	return 1;
}

int surface_loadmem(lua_State* L)
{
	size_t datasz;
	const char* data = luaL_checklstring(L, 1, &datasz);
	SDL_RWops* src = SDL_RWFromConstMem(data, datasz);
	SDL_Surface* surf = IMG_Load_RW(src, 0);
	SDL_RWclose(src);
	if (!surf)
	{
		return IMGError(L, IMG_Load_RW);
	}
	put_surface(L, surf);
	return 1;
}

void InitSurface(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_newtable(L);
	lua_setfield_function(L, "load", surface_load);
	lua_setfield_function(L, "loadmem", surface_loadmem);
	lua_setfield(L, -2, "Surface");
	lua_pop(L, 2);
}
