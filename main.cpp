#include "LuaEngine.h"
#include <string>
#include <iostream>
#include <direct.h>
using namespace std;

string LoadFile(const string& filename)
{
	string temp;
	int ret;
	char buff[1024] = { 0 };
	SDL_RWops* io = SDL_RWFromFile(filename.c_str(), "rb");
	if (!io)
	{
		SDL_Log("SDL_RWFromFile: %s\n", SDL_GetError());
		return "";
	}
	while ((ret=SDL_RWread(io, buff, 1, 1024)))
	{
		temp.append(buff, ret);
		memset(buff, 0, 1024);
	}
	SDL_RWclose(io);
	return temp;
}

int main()
{
	InitEngine();
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	InitLuaEngine(L);
	_chdir("game");
	string code = LoadFile("app.lua");
	if (luaL_loadbufferx(L, code.c_str(), code.size(), "ProgramMain", "t"))
	{
		size_t errlen;
		const char* err = lua_tolstring(L, -1, &errlen);
		string errmsg(err, errlen);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[LuaAppSyntaxError] %s", errmsg.c_str());
	}
	else if(lua_pcall(L, 0, LUA_MULTRET, 0))
	{
		size_t errlen;
		const char* err = lua_tolstring(L, -1, &errlen);
		string errmsg(err, errlen);
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[LuaAppRuntimeError] %s", errmsg.c_str());
	}
	lua_close(L);
	CloseEngine();
	return 0;
}
