#pragma once

#include "netModule.h"

class NetworkSettings
{
public:
	NetworkSettings(ULONG open_addr, USHORT open_port);
	~NetworkSettings();

	void create_iocp();
	void accept_ex_iocp();

	HANDLE get_iocp_handle() { return h_iocp; }

private:
	HANDLE h_iocp;
	WSADATA WSAData;
	int error_code;
	SOCKADDR_IN server_addr;
	SOCKADDR_IN client_addr;

	OverlappedExpansion accept_overlapped_expansion;
	SOCKET server_socket, client_socket;
};

