#include "include.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00  // Win10
#include <winsock2.h>
#include <ws2tcpip.h>

int network_getip(lua_State* L)
{
	const char* host = luaL_checkstring(L, 1);
	addrinfo* result = NULL;
	
	if (getaddrinfo(host, NULL, NULL, &result) < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "getaddrinfo");
		return lua_error(L);
	}

	int cnt = 0;
	for (addrinfo* pinfo = result; pinfo; pinfo = pinfo->ai_next)
	{
		switch (pinfo->ai_family)
		{
		case AF_INET:
		{
			char buffer[1024] = { 0 };
			if (inet_ntop(AF_INET, &((sockaddr_in*)(pinfo->ai_addr))->sin_addr, buffer, 1024))
			{
				int errcode = WSAGetLastError();
				put_winerror(L, errcode, "inet_ntop");
				freeaddrinfo(result);
				return lua_error(L);
			}
			lua_pushstring(L, buffer);
			cnt++;

			break;
		}
		}
	}

	freeaddrinfo(result);
	return cnt;
}

void InitNetwork(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_newtable(L);
	lua_setfield_function(L, "getip", network_getip);
	lua_setfield(L, -2, "network");
	lua_pop(L, 2);
}

#endif // End of _WIN32
