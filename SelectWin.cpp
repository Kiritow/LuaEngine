#include "Select.h"
#include "TCP.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00  // Win10
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
using namespace std;

int select_call(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	luaL_checktype(L, 3, LUA_TTABLE);
	int us = luaL_checkinteger(L, 4);
	FD_SET readset, writeset, errorset;

	FD_ZERO(&readset);
	lua_pushnil(L);
	while (lua_next(L, 1))
	{
		lua_pop(L, 1);
		auto s = lua_checkblock<TCPSocket>(L, -1, "LuaEngineTCPSocket");
		if (s->fd > 0)
		{
			FD_SET(s->fd, &readset);
		}
	}

	FD_ZERO(&writeset);
	lua_pushnil(L);
	while (lua_next(L, 2))
	{
		lua_pop(L, 1);
		auto s = lua_checkblock<TCPSocket>(L, -1, "LuaEngineTCPSocket");
		if (s->fd > 0)
		{
			FD_SET(s->fd, &writeset);
		}
	}

	FD_ZERO(&errorset);
	lua_pushnil(L);
	while (lua_next(L, 3))
	{
		lua_pop(L, 1);
		auto s = lua_checkblock<TCPSocket>(L, -1, "LuaEngineTCPSocket");
		if (s->fd > 0)
		{
			FD_SET(s->fd, &errorset);
		}
	}

	timeval tm;
	tm.tv_sec = us / 1000000;
	tm.tv_usec = us % 1000000;

	printf("[DEBUG] select() start\n");
	int ret = select(1024, &readset, &writeset, &errorset, &tm);
	printf("[DEBUG] select() done. ret=%d\n", ret);

	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "select");
		return lua_error(L);
	}
	else if (ret == 0)
	{
		return 0;
	}
	else
	{
		lua_newtable(L);  // ... RetT
		lua_pushnil(L);  // ... RetT nil
		while (lua_next(L, 1))
		{
			lua_pop(L, 1);  // ... RetT Socket
			auto s = lua_checkblock<TCPSocket>(L, -1, "LuaEngineTCPSocket");
			if (s->fd > 0 && FD_ISSET(s->fd, &readset))
			{
				lua_pushvalue(L, -1);  // ...  RetT Socket Socket
				lua_pushboolean(L, true);  // ...  RetT Socket Socket true
				lua_settable(L, -4);  // ...  RetT Socket
			}
		}

		lua_newtable(L);  // ... RetT
		lua_pushnil(L);  // ... RetT nil
		while (lua_next(L, 2))
		{
			lua_pop(L, 1);  // ... RetT Socket
			auto s = lua_checkblock<TCPSocket>(L, -1, "LuaEngineTCPSocket");
			if (s->fd > 0 && FD_ISSET(s->fd, &writeset))
			{
				lua_pushvalue(L, -1);  // ...  RetT Socket Socket
				lua_pushboolean(L, true);  // ...  RetT Socket Socket true
				lua_settable(L, -4);  // ...  RetT Socket
			}
		}

		lua_newtable(L);  // ... RetT
		lua_pushnil(L);  // ... RetT nil
		while (lua_next(L, 3))
		{
			lua_pop(L, 1);  // ... RetT Socket
			auto s = lua_checkblock<TCPSocket>(L, -1, "LuaEngineTCPSocket");
			if (s->fd > 0 && FD_ISSET(s->fd, &errorset))
			{
				lua_pushvalue(L, -1);  // ...  RetT Socket Socket
				lua_pushboolean(L, true);  // ...  RetT Socket Socket true
				lua_settable(L, -4);  // ...  RetT Socket
			}
		}

		return 3;  // ... RetT RetT RetT
	}
}

void InitSocketSelector(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_pushcfunction(L, select_call);
	lua_setfield(L, -2, "select");
	lua_pop(L, 2);
}

#endif // End of _WIN32
