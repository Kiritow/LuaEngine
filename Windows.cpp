#include "include.h"

#ifdef _WIN32
#include <Windows.h>

void put_winerror(lua_State* L, int errcode, const char* hint)
{
	char buff[4096] = { 0 };
	int buffsz = 4096;
	int offset = 0;
	if (hint)
	{
		sprintf(buff, "%s: ", hint);
		buffsz -= strlen(buff);
		offset += strlen(buff);
	}
	int sz = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errcode, 0, buff + offset, buffsz, NULL);
	if (!sz)
	{
		lua_pushfstring(L, "FormatMessagA: %d. Previous error: %d", GetLastError(), errcode);
	}
	else
	{
		lua_pushlstring(L, buff, offset + sz);
	}
}

void PlatInit()
{
	WORD wd = MAKEWORD(2, 2);
	WSAData wdt;
	if (WSAStartup(wd, &wdt) < 0)
	{
		printf("Unable to init winsock 2.2: WSAGetLastError: %d\n", WSAGetLastError());
	}
}

#endif  // end of ifdef _WIN32
