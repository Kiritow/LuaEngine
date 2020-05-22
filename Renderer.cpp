#include "include.h"

/*
class Renderer
	constructor(wnd: Window)
	load(filename: string): Texture
	copy(text: Texture, sx: int, sy: int, sw: int, sh: int, dx: int, dy: int, dw: int, dh: int)
	copyTo(text: Texture, x: int, y: int, [w: int, h: int])
	copyFill(text: Texture, x: int, y: int, w: int, h: int)
	copyFullFill(text: Texture)
*/

int render_close(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	SDL_DestroyRenderer(rnd);
	return 0;
}

int render_load(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	const char* filename = luaL_checkstring(L, 2);
	SDL_Texture* text = IMG_LoadTexture(rnd, filename);
	if (!text)
	{
		return IMGError(L, IMG_LoadTexture);
	}
	put_texture(L, text);
	return 1;
}

int render_clear(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	if (SDL_RenderClear(rnd) != 0)
	{
		return SDLError(L, SDL_RenderClear);
	}
	return 0;
}

int render_update(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	SDL_RenderPresent(rnd);
	return 0;
}

int render_copy(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	auto text = lua_checkpointer<SDL_Texture>(L, 2, "LuaEngineTexture");
	int sx = luaL_checkinteger(L, 3);
	int sy = luaL_checkinteger(L, 4);
	int sw = luaL_checkinteger(L, 5);
	int sh = luaL_checkinteger(L, 6);
	int dx = luaL_checkinteger(L, 7);
	int dy = luaL_checkinteger(L, 8);
	int dw = luaL_checkinteger(L, 9);
	int dh = luaL_checkinteger(L, 10);
	SDL_Rect src;
	src.x = sx;
	src.y = sy;
	src.w = sw;
	src.h = sh;
	SDL_Rect dst;
	dst.x = dx;
	dst.y = dy;
	dst.w = dw;
	dst.h = dh;
	if (SDL_RenderCopy(rnd, text, &src, &dst) != 0)
	{
		return SDLError(L, SDL_RenderCopy);
	}
	return 0;
}

int render_copyto(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	auto text = lua_checkpointer<SDL_Texture>(L, 2, "LuaEngineTexture");
	int x = luaL_checkinteger(L, 3);
	int y = luaL_checkinteger(L, 4);

	int w, h;
	if (SDL_QueryTexture(text, NULL, NULL, &w, &h) != 0)
	{
		return SDLError(L, SDL_QueryTexture);
	}

	if (!lua_isnone(L, 5))
	{
		w = luaL_checkinteger(L, 5);
	}
	if (!lua_isnone(L, 6))
	{
		h = luaL_checkinteger(L, 6);
	}
	
	SDL_Rect r;
	r.w = w;
	r.h = h;
	r.x = x;
	r.y = y;
	if (SDL_RenderCopy(rnd, text, NULL, &r) != 0)
	{
		return SDLError(L, SDL_RenderCopy);
	}
	return 0;
}

int render_copyfill(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	auto text = lua_checkpointer<SDL_Texture>(L, 2, "LuaEngineTexture");
	int x = luaL_checkinteger(L, 3);
	int y = luaL_checkinteger(L, 4);
	int w = luaL_checkinteger(L, 5);
	int h = luaL_checkinteger(L, 6);
	SDL_Rect r;
	r.w = w;
	r.h = h;
	r.x = x;
	r.y = y;
	if (SDL_RenderCopy(rnd, text, &r, NULL) != 0)
	{
		return SDLError(L, SDL_RenderCopy);
	}
	return 0;
}

int render_copyfullfill(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	auto text = lua_checkpointer<SDL_Texture>(L, 2, "LuaEngineTexture");
	if (SDL_RenderCopy(rnd, text, NULL, NULL) != 0)
	{
		return SDLError(L, SDL_RenderCopy);
	}
	return 0;
}

int render_new(lua_State* L)
{
	auto wnd = lua_checkpointer<SDL_Window>(L, 1, "LuaEngineWindow");

	SDL_Renderer* rnd = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
	if (!rnd)
	{
		return SDLError(L, SDL_CreateRenderer);
	}
	lua_newpointer(L, rnd);
	if (luaL_newmetatable(L, "LuaEngineRenderer"))
	{
		lua_setfield_function(L, "__gc", render_close);
		lua_newtable(L);
		lua_setfield_function(L, "load", render_load);
		lua_setfield_function(L, "clear", render_clear);
		lua_setfield_function(L, "update", render_update);
		lua_setfield_function(L, "copy", render_copy);
		lua_setfield_function(L, "copyTo", render_copyto);
		lua_setfield_function(L, "copyFill", render_copyfill);
		lua_setfield_function(L, "copyFullFill", render_copyfullfill);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void InitRenderer(lua_State* L)
{
	lua_register(L, "Renderer", render_new);
}
