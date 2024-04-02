/*
[Refactoring]
	1. O.O.D
	2. update SESSION Design
*/

#pragma once

#include "stdafx.h"
#include "protocol.h"

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <map>

#include <list>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

enum TYPE { ACCEPT, RECV, SEND, LOGOUT };
enum SESSION_STATE { ST_FREE, ST_ALLOC };
enum PLAYER_STATE { ST_HOME, ST_NOTREADY, ST_READY, ST_STAGE };

class OVER_EXP {
public:
	WSAOVERLAPPED	_over;
	TYPE			c_type;
	WSABUF			_wsabuf;
	char			_send_buf[BUF_SIZE];

	OVER_EXP()
	{
		ZeroMemory(&_over, sizeof(_over));
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		c_type = RECV;
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		c_type = SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

extern class PLAYER;
extern class PARTY;

extern OVER_EXP g_a_over;
extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

extern std::array<PLAYER, MAX_USER> clients;
extern std::array<PARTY, MAX_PARTY> parties;


// Dummy Client용
extern int g_c_uid;


void worker_thread(HANDLE h_iocp);

void process_packet(int c_uid, char* packet);

void disconnect(int c_uid);