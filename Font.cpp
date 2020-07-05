#include "include.h"

/*
class Font
	constructor(filename: string, size: int)
	constructor(data: string, size: int, flag: int) flag is not used currently.
	renderText(str: utf-8 string, r: int, g: int, b: int, a: int): Surface
	renderText(rnd: Renderer, str: utf-8 string, r: int, g: int, b: int, a: int): Texture
	renderTextSolid(str: utf-8 string, r: int, g: int, b: int, a: int): Surface
	renderTextSolid(rnd: Renderer, str: utf-8 string, r: int, g: int, b: int, a: int): Texture
	renderTextShaded(str: utf-8 string, fr: int, fg: int, fb: int, fa: int, br: int, bg: int, bb: int, ba: int): Surface
	renderTextShaded(rnd: Renderer, str: utf-8 string, fr: int, fg: int, fb: int, fa: int, br: int, bg: int, bb: int, ba: int): Texture
*/

int font_close(lua_State* L)
{
	auto font = lua_checkpointer<TTF_Font>(L, 1, "LuaEngineFont");
	TTF_CloseFont(font);
	return 0;
}

int font_renderutf8(lua_State* L)
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
		SDL_Surface* surf = TTF_RenderUTF8_Blended(font, str, color);
		if (!surf)
		{
			return TTFError(L, TTF_RenderUTF8_Blended);
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
		SDL_Surface* surf = TTF_RenderUTF8_Blended(font, str, color);
		if (!surf)
		{
			return TTFError(L, TTF_RenderUTF8_Blended);
		}
		SDL_Texture* text = SDL_CreateTextureFromSurface(rnd, surf);
		SDL_FreeSurface(surf);
		if (!text)
		{
			return SDLError(L, SDL_CreateTextureFromSurface);
		}
		put_texture(L, text);
		return 1;
	}
}

int font_renderutf8_solid(lua_State* L)
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
		SDL_Surface* surf = TTF_RenderUTF8_Solid(font, str, color);
		if (!surf)
		{
			return TTFError(L, TTF_RenderUTF8_Solid);
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
		SDL_Surface* surf = TTF_RenderUTF8_Solid(font, str, color);
		if (!surf)
		{
			return TTFError(L, TTF_RenderUTF8_Solid);
		}
		SDL_Texture* text = SDL_CreateTextureFromSurface(rnd, surf);
		SDL_FreeSurface(surf);
		if (!text)
		{
			return SDLError(L, SDL_CreateTextureFromSurface);
		}
		put_texture(L, text);
		return 1;
	}
}

int font_renderutf8_shaded(lua_State* L)
{
	auto font = lua_checkpointer<TTF_Font>(L, 1, "LuaEngineFont");
	const char* str;
	SDL_Color frontColor;
	SDL_Color backColor;
	if (lua_isstring(L, 2))
	{
		str = luaL_checkstring(L, 2);
		frontColor.r = luaL_checkinteger(L, 3);
		frontColor.g = luaL_checkinteger(L, 4);
		frontColor.b = luaL_checkinteger(L, 5);
		frontColor.a = luaL_checkinteger(L, 6);
		backColor.r = luaL_checkinteger(L, 7);
		backColor.g = luaL_checkinteger(L, 8);
		backColor.b = luaL_checkinteger(L, 9);
		backColor.a = luaL_checkinteger(L, 10);
		SDL_Surface* surf = TTF_RenderUTF8_Shaded(font, str, frontColor, backColor);
		if (!surf)
		{
			return TTFError(L, TTF_RenderUTF8_Shaded);
		}
		put_surface(L, surf);
		return 1;
	}
	else
	{
		auto rnd = lua_checkpointer<SDL_Renderer>(L, 2, "LuaEngineRenderer");
		str = luaL_checkstring(L, 3);
		frontColor.r = luaL_checkinteger(L, 4);
		frontColor.g = luaL_checkinteger(L, 5);
		frontColor.b = luaL_checkinteger(L, 6);
		frontColor.a = luaL_checkinteger(L, 7);
		backColor.r = luaL_checkinteger(L, 8);
		backColor.g = luaL_checkinteger(L, 9);
		backColor.b = luaL_checkinteger(L, 10);
		backColor.a = luaL_checkinteger(L, 11);
		SDL_Surface* surf = TTF_RenderUTF8_Shaded(font, str, frontColor, backColor);
		if (!surf)
		{
			return TTFError(L, TTF_RenderUTF8_Shaded);
		}
		SDL_Texture* text = SDL_CreateTextureFromSurface(rnd, surf);
		SDL_FreeSurface(surf);
		if (!text)
		{
			return SDLError(L, SDL_CreateTextureFromSurface);
		}
		put_texture(L, text);
		return 1;
	}
}

int font_new(lua_State* L)
{
	TTF_Font* font;
	if (lua_gettop(L) >= 3)
	{
		size_t datalen;
		const char* data = luaL_checklstring(L, 1, &datalen);
		int fontsize = luaL_checkinteger(L, 2);
		int flag = luaL_checkinteger(L, 3);
		SDL_RWops* src = SDL_RWFromConstMem(data, datalen);
		font = TTF_OpenFontRW(src, 0, fontsize);
		SDL_RWclose(src);
		if (!font)
		{
			return TTFError(L, TTF_OpenFontRW);
		}
	}
	else
	{
		const char* filename = luaL_checkstring(L, 1);
		int fontsize = luaL_checkinteger(L, 2);
		font = TTF_OpenFont(filename, fontsize);
		if (!font)
		{
			return TTFError(L, TTF_OpenFont);
		}
	}

	lua_newpointer(L, font);
	if (luaL_newmetatable(L, "LuaEngineFont"))
	{
		lua_setfield_function(L, "__gc", font_close);
		lua_newtable(L);
		lua_setfield_function(L, "renderText", font_renderutf8);
		lua_setfield_function(L, "renderTextSolid", font_renderutf8_solid);
		lua_setfield_function(L, "renderTextShaded", font_renderutf8_shaded);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void InitFont(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_pushcfunction(L, font_new);
	lua_setfield(L, -2, "Font");
	lua_pop(L, 2);
}
