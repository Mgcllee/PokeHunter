#pragma once
#include "protocol.h"

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

void worker_thread(HANDLE h_iocp);
void process_packet();