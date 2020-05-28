#include "include.h"
#include <vector>

class SocketData
{
public:
	int fd;
	bool nonblocking;
	std::vector<char> data;
	int buffsz;
};
