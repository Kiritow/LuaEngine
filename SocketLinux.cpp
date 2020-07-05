#include "Socket.h"

#ifndef _WIN32

int SetSocketBlocking(lua_State* L, int fd, bool blocking)
{
    const int flags = fcntl(fd, F_GETFL, 0);
    if (((flags & O_NONBLOCK) && !blocking) || (!(flags & O_NONBLOCK) && blocking))
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

#endif
