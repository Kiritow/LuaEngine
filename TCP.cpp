#include "TCP.h"
#include <cstring>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00  // Win10
#include <winsock2.h>
#include <ws2tcpip.h>

static int SetSocketBlocking(lua_State* L, int fd, bool blocking)
{
    u_long arg = blocking ? 0 : 1;
    int ret = ioctlsocket(fd, FIONBIO, &arg);
    if (ret)
    {
        int errcode = WSAGetLastError();
        put_winerror(L, errcode, "ioctlsocket");
        return lua_error(L);
    }
    return 0;
}

#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

static int SetSocketBlocking(lua_State* L, int fd, bool blocking)
{
    const int flags = fcntl(fd, F_GETFL, 0);
    if( ((flags & O_NONBLOCK) && !blocking) || (!(flags & O_NONBLOCK) && blocking))
    {
        return 0;
    }
    int ret = fcntl(fd, F_SETFL, blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
    if (ret)
    {
        put_linuxerror(L, errno, "fcntl");
        return lua_error(L);
    }
    return 0;
}

#define closesocket close
#define WSAGetLastError() errno
#define WSAEWOULDBLOCK EWOULDBLOCK
#define put_winerror put_linuxerror

#endif

using namespace std;


int tcp_socket_dtor(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	if (s->fd > 0)
	{
		closesocket(s->fd);
		s->fd = -1;
	}
	s->~SocketData();
	return 0;
}

int tcp_socket_close(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	if (s->fd > 0)
	{
		closesocket(s->fd);
		s->fd = -1;
	}
	return 0;
}

int tcp_socket_send(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	size_t sz;
	const char* str = luaL_checklstring(L, 2, &sz);
	size_t done = 0;
	while (done < sz)
	{
		int ret = send(s->fd, str + done, sz - done, 0);
		if (ret < 0)
		{
			int errcode = WSAGetLastError();
			if (s->nonblocking && errcode == WSAEWOULDBLOCK)
			{
				lua_pushinteger(L, done);
				lua_pushboolean(L, true);
				return 2;
			}
			put_winerror(L, errcode, "send");
			return lua_error(L);
		}
		done += ret;
	}
	lua_pushinteger(L, done);
	return 1;
}

int tcp_socket_recv(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	int ret = recv(s->fd, s->data.data(), s->buffsz, 0);
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		if (s->nonblocking && errcode == WSAEWOULDBLOCK)
		{
			lua_pushstring(L, "");
			lua_pushboolean(L, true);
			return 2;
		}
		put_winerror(L, errcode, "recv");
		return lua_error(L);
	}
	lua_pushlstring(L, s->data.data(), ret);
	return 1;
}

int tcp_socket_connect(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	const char* ip = luaL_checkstring(L, 2);
	int port = luaL_checkinteger(L, 3);

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, ip, &addr.sin_addr) < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "inet_pton");
		return lua_error(L);
	}
	addr.sin_port = htons(port);

	int ret = connect(s->fd, (const sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		if (s->nonblocking && errcode == WSAEWOULDBLOCK)
		{
			lua_pushboolean(L, true);
			return 1;
		}
		put_winerror(L, errcode, "connect");
		return lua_error(L);
	}

	return 0;
}

int tcp_socket_listen(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	const char* ip = NULL;
	int port;
	int backlog = 10;
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		ip = luaL_checkstring(L, 2);
		port = luaL_checkinteger(L, 3);
		if (!lua_isnone(L, 4))
		{
			backlog = luaL_checkinteger(L, 4);
		}
	}
	else
	{
		port = luaL_checkinteger(L, 2);
		if (!lua_isnone(L, 3))
		{
			backlog = luaL_checkinteger(L, 3);
		}
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (ip && strcmp(ip, "0.0.0.0"))
	{
		if (inet_pton(AF_INET, ip, &addr.sin_addr) < 0)
		{
			int errcode = WSAGetLastError();
			put_winerror(L, errcode, "inet_pton");
			return lua_error(L);
		}
	}
	else
	{
		addr.sin_addr.s_addr = INADDR_ANY;
	}
	addr.sin_port = htons(port);

	printf("fd: %d ip: %s port: %d\n", s->fd, ip, port);

	int ret = bind(s->fd, (const sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "bind");
		return lua_error(L);
	}
	ret = listen(s->fd, backlog);
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "listen");
		return lua_error(L);
	}
	return 0;
}

int tcp_socket_setblocking(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	luaL_checkany(L, 2);
	bool blocking = lua_toboolean(L, 2);
	if (blocking != s->nonblocking)
	{
		return 0;
	}

	SetSocketBlocking(L, s->fd, blocking);
	return 0;
}

int tcp_socket_accept(lua_State* L);

void put_tcp_socket(lua_State* L, int fd, bool nonblocking)
{
	auto s = new (lua_newblock<SocketData>(L)) SocketData;
	if (luaL_newmetatable(L, "LuaEngineTCPSocket"))
	{
		lua_setfield_function(L, "__gc", tcp_socket_dtor);
		lua_newtable(L);
		lua_setfield_function(L, "close", tcp_socket_close);
		lua_setfield_function(L, "send", tcp_socket_send);
		lua_setfield_function(L, "recv", tcp_socket_recv);
		lua_setfield_function(L, "listen", tcp_socket_listen);
		lua_setfield_function(L, "connect", tcp_socket_connect);
		lua_setfield_function(L, "accept", tcp_socket_accept);
		lua_setfield_function(L, "setblocking", tcp_socket_setblocking);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	s->fd = fd;
	s->nonblocking = nonblocking;
	s->buffsz = 4096;
	s->data.resize(s->buffsz);
}

int tcp_socket_accept(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineTCPSocket");
	sockaddr_in addr;
	socklen_t addrsz = sizeof(addr);
	int ret = accept(s->fd, (sockaddr*)&addr, &addrsz);
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		if (s->nonblocking && errcode == WSAEWOULDBLOCK)
		{
			return 0;
		}
		put_winerror(L, errcode, "accept");
		return lua_error(L);
	}
	put_tcp_socket(L, ret, false);

	char buffer[1024] = { 0 };
	if (inet_ntop(AF_INET, &addr.sin_addr, buffer, 1024) < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "inet_ntop");
		return lua_error(L);
	}
	lua_pushstring(L, buffer);
	lua_pushinteger(L, ntohs(addr.sin_port));
	return 3;
}

int tcp_socket_new(lua_State* L)
{
	bool nonblocking = false;
	if (!lua_isnone(L, 1))
	{
		if (lua_toboolean(L, 1))
		{
			nonblocking = true;
		}
	}
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "socket");
		return lua_error(L);
	}
	if (nonblocking)
	{
		SetSocketBlocking(L, fd, false);
	}
	put_tcp_socket(L, fd, nonblocking);
	return 1;
}

void InitTCPSocket(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_pushcfunction(L, tcp_socket_new);
	lua_setfield(L, -2, "TCPSocket");
	lua_pop(L, 2);
}
