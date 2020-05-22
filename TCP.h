#include "include.h"
#include <vector>

/*
class TCPSocket
	constructor(nonblocking: int)
	close()
	listen([ip: string], port: int, [backlog: int])  Address default to 0.0.0.0, backlog default to 10.

	# Blocking mode: All non-expected error will be raised as exception.
	connect(ip: string, port: int)
	accept(): TCPSocket PeerIP PeerPort
	send(data: string)  All data will be sent before return.
	recv([maxsize: int]): string.  Default to 4KB.

	# Non-blocking mode:
	connect(ip: string, port: int)
	accept(): (TCPSocket PeerIP PeerPort) or nil
	send(data: string)
	recv([maxsize: int]): string
*/
