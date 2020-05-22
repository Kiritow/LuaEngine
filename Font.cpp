#include "include.h"

/*
class Font
	constructor(filename: string, size: int)
	renderText(str: string, r: int, g: int, b: int, a: int): Surface
	renderText(rnd: Renderer, str: string, r: int, g: int, b: int, a: int): Texture
*/

int font_close(lua_State* L)
{
	auto font = lua_checkpointer<TTF_Font>(L, 1, "LuaEngineFont");
	TTF_CloseFont(font);
	return 0;
}

int font_rendertext(lua_State* L)
{
	auto font = lua_checkpointer<TTF_Font>(L, 1, "LuaEngineFont");
	const char* str;
	SDL_Color color;
	if (lua_isstring(L, 2))
	{
		str = luaL_checkstring(L, 2);
		color.r = luaL_checkinteger(L, 3);
		color.g = luaL_checkinteger(L, 4);
		color.b = luaL_checkinteger(L, 5);
		color.a = luaL_checkinteger(L, 6);
		SDL_Surface* surf = TTF_RenderText_Blended(font, str, color);
		if (!surf)
		{
			return TTFError(L, TTF_RenderText_Blended);
		}
		put_surface(L, surf);
		return 1;
	}
	else
	{
		auto rnd = lua_checkpointer<SDL_Renderer>(L, 2, "LuaEngineRenderer");
		str = luaL_checkstring(L, 3);
		color.r = luaL_checkinteger(L, 4);
		color.g = luaL_checkinteger(L, 5);
		color.b = luaL_checkinteger(L, 6);
		color.a = luaL_checkinteger(L, 7);
		SDL_Surface* surf = TTF_RenderText_Blended(font, str, color);
		if (!surf)
		{
			return TTFError(L, TTF_RenderText_Blended);
		}
		SDL_Texture* text = SDL_CreateTextureFromSurface(rnd, surf);
		if (!text)
		{
			SDL_FreeSurface(surf);
			return SDLError(L, SDL_CreateTextureFromSurface);
		}
		put_texture(L, text);
		return 1;
	}
}

int font_new(lua_State* L)
{
	const char* filename = luaL_checkstring(L, 1);
	int fontsize = luaL_checkinteger(L, 2);
	TTF_Font* font = TTF_OpenFont(filename, fontsize);
	if (!font)
	{
		return TTFError(L, TTF_OpenFont);
	}
	lua_newpointer(L, font);
	if (luaL_newmetatable(L, "LuaEngineFont"))
	{
		lua_setfield_function(L, "__gc", font_close);
		lua_newtable(L);
		lua_setfield_function(L, "renderText", font_rendertext);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void InitFont(lua_State* L)
{
	lua_register(L, "Font", font_new);
}
