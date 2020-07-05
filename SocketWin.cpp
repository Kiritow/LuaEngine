#include "Socket.h"

#ifdef _WIN32

int SetSocketBlocking(lua_State* L, int fd, bool blocking)
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

#endif
