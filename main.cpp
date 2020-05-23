#include "LuaEngine.h"
#include <string>
#include <iostream>
using namespace std;

string LoadFile(const string& filename)
{
	string temp;
	int ret;
	char buff[1024];
	SDL_RWops* io = SDL_RWFromFile(filename.c_str(), "rb");
	if (!io)
	{
		SDL_Log("SDL_RWFromFile: %s\n", SDL_GetError());
		return "";
	}
	while ((ret=SDL_RWread(io, buff, 1, 1024)))
	{
		temp.append(buff, ret);
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
	string code = LoadFile("game/app.lua");
	if (luaL_loadstring(L, code.c_str()))
	{
		cout << lua_tostring(L, -1) << endl;
		SDL_Log("[LuaAppSyntaxError] %s\n", lua_tostring(L, -1));
	}
	else if(lua_pcall(L, 0, LUA_MULTRET, 0))
	{
		cout << lua_tostring(L, -1) << endl;
		SDL_Log("[LuaAppRuntimeError] %s\n", lua_tostring(L, -1));
	}
	lua_close(L);
	CloseEngine();
	return 0;
}
