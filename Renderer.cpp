#include "include.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cstring>

/*
class Renderer
	constructor(wnd: Window)
	load(filename: string): Texture
	loadmem(data: string): Texture
	render(surf: Surface): Texture
	setTarget(text: Texture)
	clear()
	update()

	copy(text: Texture, sx: int, sy: int, sw: int, sh: int, dx: int, dy: int, dw: int, dh: int)
	copyTo(text: Texture, dx: int, dy: int, [dw: int, dh: int])
	copyFill(text: Texture, sx: int, sy: int, sw: int, sh: int)
	copyFullFill(text: Texture)

	copyEx(text: Texture, sx: int, sy: int, sw: int, sh: int, dx: int, dy: int, dw: int, dh: int, angle: number, flip: string, [cx: int, cy: int])
	copyToEx(text: Texture, dx: int, dy: int, [dw: int, dh: int], angle: number, flip: string, [cx: int, cy: int])
	copyFillEx(text: Texture, sx: int, sy: int, sw: int, sh: int, angle: number, flip: string, [cx: int, cy: int])
	copyFullFillEx(text: Texture, angle: number, flip: string, [cx: int, cy: int])

	drawPoint(x: int, y: int)
	drawLine(x1: int, y1: int, x2: int, y2: int)
	drawRect(x: int, y: int, w: int, h: int)
	fillRect(x: int, y: int, w: int, h: int)
	fillTriangle(x1: int, y1: int, x2: int, y2: int, x3: int, y3: int, [r: int, g: int, b:int, a: int])  #SDL2_gfx#
	getColor(): r: int, g: int, b: int, a: int
	setColor(r: int, g: int, b: int, a: int)
	getBlendMode(): string
	setBlendMode(mode: string)
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

int render_loadmem(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	size_t len;
	const char* data = luaL_checklstring(L, 2, &len);
	SDL_RWops* src = SDL_RWFromConstMem(data, len);
	SDL_Texture* text = IMG_LoadTexture_RW(rnd, src, 0);
	SDL_RWclose(src);
	if (!text) {
		return IMGError(L, IMG_LoadTexture_RW);
	}
	put_texture(L, text);
	return 1;
}

int render_render(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	auto surf = lua_checkpointer<SDL_Surface>(L, 2, "LuaEngineSurface");
	SDL_Texture* text = SDL_CreateTextureFromSurface(rnd, surf);
	if (!text)
	{
		return SDLError(L, SDL_CreateTextureFromSurface);
	}
	put_texture(L, text);
	return 1;
}

int render_settarget(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	if (lua_gettop(L) == 1)
	{
		if (SDL_SetRenderTarget(rnd, NULL) != 0)
		{
			return SDLError(L, SDL_SetRenderTarget);
		}
		return 0;
	}
	else
	{
		auto text = lua_checkpointer<SDL_Texture>(L, 2, "LuaEngineTexture");
		if (SDL_SetRenderTarget(rnd, text) != 0)
		{
			return SDLError(L, SDL_SetRenderTarget);
		}
		return 0;
	}
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

static SDL_RendererFlip flip_string_to_enum(const char* str)
{
	int flag = SDL_FLIP_NONE;
	if (strstr(str, "vert"))
	{
		flag |= SDL_FLIP_VERTICAL;
	}
	else if (strstr(str, "hori") == 0)
	{
		flag |= SDL_FLIP_HORIZONTAL;
	}
	return (SDL_RendererFlip)flag;
}

int render_copyex(lua_State* L)
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
	double angle = luaL_checknumber(L, 11);
	const char* rawflip = luaL_checkstring(L, 12);
	SDL_RendererFlip flip = flip_string_to_enum(rawflip);

	SDL_Point* pCenter, center;
	if (!lua_isnone(L, 13))
	{
		center.x = luaL_checkinteger(L, 13);
		center.y = luaL_checkinteger(L, 14);
		pCenter = &center;
	}
	else
	{
		pCenter = nullptr;
	}

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

	if (SDL_RenderCopyEx(rnd, text, &src, &dst, angle, pCenter, flip) != 0)
	{
		return SDLError(L, SDL_RenderCopyEx);
	}

	return 0;
}

int render_copytoex(lua_State* L)
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

	double angle;
	const char* rawflip;
	SDL_Point* pCenter, center;

	if (!lua_isnone(L, 6) && lua_type(L, 6) == LUA_TSTRING)
	{
		angle = luaL_checknumber(L, 5);
		rawflip = luaL_checkstring(L, 6);

		if (!lua_isnone(L, 7))
		{
			center.x = luaL_checkinteger(L, 7);
			center.y = luaL_checkinteger(L, 8);
			pCenter = &center;
		}
		else
		{
			pCenter = nullptr;
		}
	}
	else if (!lua_isnone(L, 7) && lua_type(L, 7) == LUA_TSTRING)
	{
		w = luaL_checkinteger(L, 5);
		angle = luaL_checknumber(L, 6);
		rawflip = luaL_checkstring(L, 7);

		if (!lua_isnone(L, 8))
		{
			center.x = luaL_checkinteger(L, 8);
			center.y = luaL_checkinteger(L, 9);
			pCenter = &center;
		}
		else
		{
			pCenter = nullptr;
		}
	}
	else
	{
		w = luaL_checkinteger(L, 5);
		h = luaL_checkinteger(L, 6);

		angle = luaL_checknumber(L, 7);
		rawflip = luaL_checkstring(L, 8);
		if (!lua_isnone(L, 9))
		{
			center.x = luaL_checkinteger(L, 9);
			center.y = luaL_checkinteger(L, 10);
			pCenter = &center;
		}
		else
		{
			pCenter = nullptr;
		}
	}

	SDL_RendererFlip flip = flip_string_to_enum(rawflip);

	SDL_Rect r;
	r.w = w;
	r.h = h;
	r.x = x;
	r.y = y;

	if (SDL_RenderCopyEx(rnd, text, NULL, &r, angle, pCenter, flip) != 0)
	{
		return SDLError(L, SDL_RenderCopyEx);
	}

	return 0;
}

int render_copyfillex(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	auto text = lua_checkpointer<SDL_Texture>(L, 2, "LuaEngineTexture");
	int x = luaL_checkinteger(L, 3);
	int y = luaL_checkinteger(L, 4);
	int w = luaL_checkinteger(L, 5);
	int h = luaL_checkinteger(L, 6);
	double angle = luaL_checknumber(L, 7);
	const char* rawflip = luaL_checkstring(L, 8);
	SDL_RendererFlip flip = flip_string_to_enum(rawflip);

	SDL_Point* pCenter, center;
	if (!lua_isnone(L, 9))
	{
		center.x = luaL_checkinteger(L, 9);
		center.y = luaL_checkinteger(L, 10);
		pCenter = &center;
	}
	else
	{
		pCenter = nullptr;
	}

	SDL_Rect r;
	r.w = w;
	r.h = h;
	r.x = x;
	r.y = y;

	if (SDL_RenderCopyEx(rnd, text, &r, NULL, angle, pCenter, flip) != 0)
	{
		return SDLError(L, SDL_RenderCopyEx);
	}
	return 0;
}

int render_copyfullfillex(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	auto text = lua_checkpointer<SDL_Texture>(L, 2, "LuaEngineTexture");
	double angle = luaL_checknumber(L, 3);
	const char* rawflip = luaL_checkstring(L, 4);
	SDL_RendererFlip flip = flip_string_to_enum(rawflip);

	SDL_Point* pCenter, center;
	if (!lua_isnone(L, 5))
	{
		center.x = luaL_checkinteger(L, 5);
		center.y = luaL_checkinteger(L, 6);
		pCenter = &center;
	}
	else
	{
		pCenter = nullptr;
	}

	if (SDL_RenderCopyEx(rnd, text, NULL, NULL, angle, pCenter, flip) != 0)
	{
		return SDLError(L, SDL_RenderCopyEx);
	}
	return 0;
}

int render_drawpoint(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	if (SDL_RenderDrawPoint(rnd, x, y))
	{
		return SDLError(L, SDL_RenderDrawPoint);
	}
	return 0;
}

int render_drawline(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	int x1 = luaL_checkinteger(L, 2);
	int y1 = luaL_checkinteger(L, 3);
	int x2 = luaL_checkinteger(L, 4);
	int y2 = luaL_checkinteger(L, 5);
	if (SDL_RenderDrawLine(rnd, x1, y1, x2, y2) != 0)
	{
		return SDLError(L, SDL_RenderDrawLine);
	}
	return 0;
}

int render_drawrect(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	SDL_Rect r;
	r.x = luaL_checkinteger(L, 2);
	r.y = luaL_checkinteger(L, 3);
	r.w = luaL_checkinteger(L, 4);
	r.h = luaL_checkinteger(L, 5);
	if (SDL_RenderDrawRect(rnd, &r) != 0)
	{
		return SDLError(L, SDL_RenderDrawRect);
	}
	return 0;
}

int render_fillrect(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	SDL_Rect r;
	r.x = luaL_checkinteger(L, 2);
	r.y = luaL_checkinteger(L, 3);
	r.w = luaL_checkinteger(L, 4);
	r.h = luaL_checkinteger(L, 5);
	if (SDL_RenderFillRect(rnd, &r) != 0)
	{
		return SDLError(L, SDL_RenderFillRect);
	}
	return 0;
}

int render_filltriangle(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");

	int x1 = luaL_checkinteger(L, 2);
	int y1 = luaL_checkinteger(L, 3);
	int x2 = luaL_checkinteger(L, 4);
	int y2 = luaL_checkinteger(L, 5);
	int x3 = luaL_checkinteger(L, 6);
	int y3 = luaL_checkinteger(L, 7);

	Uint8 r, g, b, a;
	if (SDL_GetRenderDrawColor(rnd, &r, &g, &b, &a))
	{
		return SDLError(L, SDL_GetRenderDrawColor);
	}
	SDL_BlendMode mode;
	if (SDL_GetRenderDrawBlendMode(rnd, &mode))
	{
		return SDLError(L, SDL_GetRenderDrawBlendMode);
	}

	Uint8 rr, gg, bb, aa;
	rr = lua_isnone(L, 8) ? r : luaL_checkinteger(L, 8);
	gg = lua_isnone(L, 9) ? g : luaL_checkinteger(L, 9);
	bb = lua_isnone(L, 10) ? b : luaL_checkinteger(L, 10);
	aa = lua_isnone(L, 11) ? a : luaL_checkinteger(L, 11);

	int ret = filledTrigonRGBA(rnd, x1, y1, x2, y2, x3, y3, rr, gg, bb, aa);

	if (SDL_SetRenderDrawBlendMode(rnd, mode))
	{
		return SDLError(L, SDL_SetRenderDrawBlendMode);
	}
	if (SDL_SetRenderDrawColor(rnd, r, g, b, a))
	{
		return SDLError(L, SDL_SetRenderDrawColor);
	}

	if(ret)
	{
		return SDLError(L, filledTrigonRGBA);
	}

	return 0;
}

int render_getblendmode(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	SDL_BlendMode mode;
	if (SDL_GetRenderDrawBlendMode(rnd, &mode))
	{
		return SDLError(L, SDL_GetRenderDrawBlendMode);
	}
	switch (mode)
	{
	case SDL_BlendMode::SDL_BLENDMODE_ADD:
		lua_pushstring(L, "add");
		break;
	case SDL_BlendMode::SDL_BLENDMODE_BLEND:
		lua_pushstring(L, "blend");
		break;
	case SDL_BlendMode::SDL_BLENDMODE_INVALID:
		lua_pushstring(L, "invalid");
		break;
	case SDL_BlendMode::SDL_BLENDMODE_MOD:
		lua_pushstring(L, "mod");
		break;
	case SDL_BlendMode::SDL_BLENDMODE_MUL:
		lua_pushstring(L, "mul");
		break;
	case SDL_BlendMode::SDL_BLENDMODE_NONE:
		lua_pushstring(L, "none");
		break;
	default:
		lua_pushstring(L, "unknown");
		break;
	}
	return 1;
}

int render_setblendmode(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	const char* modestr = luaL_checkstring(L, 2);
	SDL_BlendMode mode;
	if (strcmp(modestr, "add") == 0)
	{
		mode = SDL_BlendMode::SDL_BLENDMODE_ADD;
	}
	else if (strcmp(modestr, "blend") == 0)
	{
		mode = SDL_BlendMode::SDL_BLENDMODE_BLEND;
	}
	else if (strcmp(modestr, "invalid") == 0)
	{
		mode = SDL_BlendMode::SDL_BLENDMODE_INVALID;
	}
	else if (strcmp(modestr, "mod") == 0)
	{
		mode = SDL_BlendMode::SDL_BLENDMODE_MOD;
	}
	else if (strcmp(modestr, "mul") == 0)
	{
		mode = SDL_BlendMode::SDL_BLENDMODE_MUL;
	}
	else if (strcmp(modestr, "none") == 0)
	{
		mode = SDL_BlendMode::SDL_BLENDMODE_NONE;
	}
	else
	{
		return 0;
	}
	if (SDL_SetRenderDrawBlendMode(rnd, mode))
	{
		return SDLError(L, SDL_SetRenderDrawBlendMode);
	}
	return 0;
}

int render_setcolor(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	int r = luaL_checkinteger(L, 2);
	int g = luaL_checkinteger(L, 3);
	int b = luaL_checkinteger(L, 4);
	int a = luaL_checkinteger(L, 5);
	if (SDL_SetRenderDrawColor(rnd, r, g, b, a))
	{
		return SDLError(L, SDL_SetRenderDrawColor);
	}
	return 0;
}

int render_getcolor(lua_State* L)
{
	auto rnd = lua_checkpointer<SDL_Renderer>(L, 1, "LuaEngineRenderer");
	uint8_t r, g, b, a;
	if (SDL_GetRenderDrawColor(rnd, &r, &g, &b, &a))
	{
		return SDLError(L, SDL_GetRenderDrawColor);
	}
	lua_pushinteger(L, r);
	lua_pushinteger(L, g);
	lua_pushinteger(L, b);
	lua_pushinteger(L, a);
	return 4;
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
		lua_setfield_function(L, "loadmem", render_loadmem);
		lua_setfield_function(L, "render", render_render);
		lua_setfield_function(L, "setTarget", render_settarget);
		lua_setfield_function(L, "clear", render_clear);
		lua_setfield_function(L, "update", render_update);
		lua_setfield_function(L, "copy", render_copy);
		lua_setfield_function(L, "copyTo", render_copyto);
		lua_setfield_function(L, "copyFill", render_copyfill);
		lua_setfield_function(L, "copyFullFill", render_copyfullfill);
		lua_setfield_function(L, "copyEx", render_copyex);
		lua_setfield_function(L, "copyToEx", render_copytoex);
		lua_setfield_function(L, "copyFillEx", render_copyfillex);
		lua_setfield_function(L, "copyFullFillEx", render_copyfullfillex);
		lua_setfield_function(L, "drawPoint", render_drawpoint);
		lua_setfield_function(L, "drawLine", render_drawline);
		lua_setfield_function(L, "drawRect", render_drawrect);
		lua_setfield_function(L, "fillRect", render_fillrect);
		lua_setfield_function(L, "fillTriangle", render_filltriangle);
		lua_setfield_function(L, "getColor", render_getcolor);
		lua_setfield_function(L, "setColor", render_setcolor);
		lua_setfield_function(L, "getBlendMode", render_getblendmode);
		lua_setfield_function(L, "setBlendMode", render_setblendmode);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

void InitRenderer(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_pushcfunction(L, render_new);
	lua_setfield(L, -2, "Renderer");
	lua_pop(L, 2);
}
