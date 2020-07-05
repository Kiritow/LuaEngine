#include "include.h"
#include <vector>


#ifdef _WIN32
#define _WIN32_WINNT 0x0A00  // Win10
#include <winsock2.h>
#include <ws2tcpip.h>

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

#define closesocket close
#define WSAGetLastError() errno
#define WSAEWOULDBLOCK EWOULDBLOCK
#define put_winerror put_linuxerror

#endif

int SetSocketBlocking(lua_State* L, int fd, bool blocking);

class SocketData
{
public:
	int fd;
	bool nonblocking;
	std::vector<char> data;
	int buffsz;
};
