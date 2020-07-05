#include "Socket.h"

/*
class UDPSocket
	constructor()
	close()
	setblocking(nonblocking: boolean)
	setbroadcast(isbroadcast: boolean)  Broadcast default to false.
	listen([ip: string], port: int) Address default to 0.0.0.0.

	connect(ip: string, port: int)
	send(data: string)
	recv([maxsize: int]): string. Default to 4KB.
	sendto([ip: string], port: int, data: string)  If ip is empty, default to 255.255.255.255 (broadcast)
	recvfrom(): data: string, ip: string, port: int
*/

int udp_socket_dtor(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	if (s->fd > 0)
	{
		closesocket(s->fd);
		s->fd = -1;
	}
	s->~SocketData();
	return 0;
}

int udp_socket_close(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	if (s->fd > 0)
	{
		closesocket(s->fd);
		s->fd = -1;
	}
	return 0;
}

int udp_socket_send(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	size_t sz;
	const char* str = luaL_checklstring(L, 2, &sz);
	int ret = send(s->fd, str, sz, 0);
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		if (s->nonblocking && errcode == WSAEWOULDBLOCK)
		{
			lua_pushboolean(L, true);
			return 1;
		}
		put_winerror(L, errcode, "send");
		return lua_error(L);
	}
	return 0;
}

int udp_socket_recv(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	int ret = recv(s->fd, s->data.data(), s->buffsz, 0);
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		if (s->nonblocking && errcode == WSAEWOULDBLOCK)
		{
			return 0;
		}
		put_winerror(L, errcode, "recv");
		return lua_error(L);
	}
	lua_pushlstring(L, s->data.data(), ret);
	return 1;
}

int udp_socket_sendto(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	const char* ip = NULL;
	int port;
	size_t sz;
	const char* str = NULL;
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		ip = luaL_checkstring(L, 2);
		port = luaL_checkinteger(L, 3);
		str = luaL_checklstring(L, 4, &sz);
	}
	else
	{
		port = luaL_checkinteger(L, 2);
		str = luaL_checklstring(L, 3, &sz);
	}
	
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (ip && strcmp(ip, "255.255.255.255"))
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
		addr.sin_addr.s_addr = INADDR_BROADCAST;
	}
	int ret = sendto(s->fd, str, sz, 0, (const sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		if (s->nonblocking && errcode == WSAEWOULDBLOCK)
		{
			lua_pushboolean(L, true);
			return 1;
		}
		put_winerror(L, errcode, "sendto");
		return lua_error(L);
	}
	return 0;
}

int udp_socket_recvfrom(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	int ret = recvfrom(s->fd, s->data.data(), s->buffsz, 0, (sockaddr*)&addr, &addrlen);
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		if (s->nonblocking && errcode == WSAEWOULDBLOCK)
		{
			return 0;
		}
		put_winerror(L, errcode, "recvfrom");
		return lua_error(L);
	}
	char buffer[1024] = { 0 };
	if (inet_ntop(AF_INET, &addr.sin_addr, buffer, 1024) < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "inet_ntop");
		return lua_error(L);
	}

	lua_pushlstring(L, s->data.data(), ret);
	lua_pushstring(L, buffer);
	lua_pushinteger(L, ntohs(addr.sin_port));
	return 3;
}

int udp_socket_connect(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
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

int udp_socket_setblocking(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	luaL_checkany(L, 2);
	bool blocking = lua_toboolean(L, 2);
	if (blocking != s->nonblocking)
	{
		return 0;
	}
	if (blocking)  // set to blocking
	{
		u_long arg = 0;
		int ret = ioctlsocket(s->fd, FIONBIO, &arg);
		if (ret)
		{
			int errcode = WSAGetLastError();
			put_winerror(L, errcode, "ioctlsocket");
			return lua_error(L);
		}
	}
	else  // set to nonblocking
	{
		u_long arg = 1;
		int ret = ioctlsocket(s->fd, FIONBIO, &arg);
		if (ret)
		{
			int errcode = WSAGetLastError();
			put_winerror(L, errcode, "ioctlsocket");
			return lua_error(L);
		}
	}
	return 0;
}

int udp_socket_setbroadcast(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	socklen_t opt = 0;
	if (!lua_isnone(L, 2) && lua_toboolean(L, 2))
	{
		opt = 1;
	}
	
	int ret = setsockopt(s->fd, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt));
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "setsockopt");
		return lua_error(L);
	}

	return 0;
}

int udp_socket_listen(lua_State* L)
{
	auto s = lua_checkblock<SocketData>(L, 1, "LuaEngineUDPSocket");
	const char* ip = NULL;
	int port;
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		ip = luaL_checkstring(L, 2);
		port = luaL_checkinteger(L, 3);
	}
	else
	{
		port = luaL_checkinteger(L, 2);
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

	int ret = bind(s->fd, (const sockaddr*)&addr, sizeof(addr));
	if (ret < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "bind");
		return lua_error(L);
	}

	return 0;
}

void put_udp_socket(lua_State* L, int fd, bool nonblocking)
{
	auto s = new (lua_newblock<SocketData>(L)) SocketData;
	if (luaL_newmetatable(L, "LuaEngineUDPSocket"))
	{
		lua_setfield_function(L, "__gc", udp_socket_dtor);
		lua_newtable(L);
		lua_setfield_function(L, "close", udp_socket_close);
		lua_setfield_function(L, "send", udp_socket_send);
		lua_setfield_function(L, "recv", udp_socket_recv);
		lua_setfield_function(L, "sendto", udp_socket_sendto);
		lua_setfield_function(L, "recvfrom", udp_socket_recvfrom);
		lua_setfield_function(L, "listen", udp_socket_listen);
		lua_setfield_function(L, "connect", udp_socket_connect);
		lua_setfield_function(L, "setblocking", udp_socket_setblocking);
		lua_setfield_function(L, "setbroadcast", udp_socket_setbroadcast);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	s->fd = fd;
	s->nonblocking = nonblocking;
	s->buffsz = 4096;
	s->data.resize(s->buffsz);
}

int udp_socket_new(lua_State* L)
{
	bool nonblocking = false;
	if (!lua_isnone(L, 1))
	{
		if (lua_toboolean(L, 1))
		{
			nonblocking = true;
		}
	}
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		int errcode = WSAGetLastError();
		put_winerror(L, errcode, "socket");
		return lua_error(L);
	}
	if (nonblocking)
	{
		u_long arg = 1;
		int ret = ioctlsocket(fd, FIONBIO, &arg);
		if (ret)
		{
			int errcode = WSAGetLastError();
			closesocket(fd);
			put_winerror(L, errcode, "ioctlsocket");
			return lua_error(L);
		}
	}
	put_udp_socket(L, fd, nonblocking);
	return 1;
}

void InitUDPSocket(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_pushcfunction(L, udp_socket_new);
	lua_setfield(L, -2, "UDPSocket");
	lua_pop(L, 2);
}
