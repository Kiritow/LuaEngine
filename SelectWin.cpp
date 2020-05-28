#include "Select.h"
#include "Socket.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00  // Win10
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
using namespace std;

inline void luatable_to_fdset(lua_State* L, int tbl_index, fd_set& fds)
{
	lua_pushnil(L);
	while (lua_next(L, tbl_index))
	{
		lua_pop(L, 1);
		auto s = lua_testblock<SocketData>(L, -1, "LuaEngineTCPSocket") ? lua_checkblock<SocketData>(L, -1, "LuaEngineTCPSocket") : lua_checkblock<SocketData>(L, -1, "LuaEngineUDPSocket");
		if (s->fd > 0)
		{
			FD_SET(s->fd, &fds);
		}
	}
}

inline void fdset_to_luatable(lua_State* L, int tbl_index, fd_set& fds)
{
	lua_newtable(L);  // ... RetT
	lua_pushnil(L);  // ... RetT nil
	while (lua_next(L, tbl_index))
	{
		lua_pop(L, 1);  // ... RetT Socket
		auto s = lua_testblock<SocketData>(L, -1, "LuaEngineTCPSocket") ? lua_checkblock<SocketData>(L, -1, "LuaEngineTCPSocket") : lua_checkblock<SocketData>(L, -1, "LuaEngineUDPSocket");
		if (s->fd > 0 && FD_ISSET(s->fd, &fds))
		{
			lua_pushvalue(L, -1);  // ...  RetT Socket Socket
			lua_pushboolean(L, true);  // ...  RetT Socket Socket true
			lua_settable(L, -4);  // ...  RetT Socket
		}
	}
}

int select_call(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	luaL_checktype(L, 3, LUA_TTABLE);
	int us = luaL_checkinteger(L, 4);
	fd_set readset, writeset, errorset;

	FD_ZERO(&readset);
	luatable_to_fdset(L, 1, readset);

	FD_ZERO(&writeset);
	luatable_to_fdset(L, 2, writeset);

	FD_ZERO(&errorset);
	luatable_to_fdset(L, 3, errorset);

	timeval tm;
	tm.tv_sec = us / 1000000;
	tm.tv_usec = us % 1000000;

	int ret = select(1024, &readset, &writeset, &errorset, &tm);

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
		fdset_to_luatable(L, 1, readset);  // +1
		fdset_to_luatable(L, 2, writeset); // +1
		fdset_to_luatable(L, 3, errorset); // +1

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
