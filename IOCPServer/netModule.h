#pragma once
#include "protocol.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <array>

using namespace std;

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

enum TYPE { ACCEPT, RECV, SEND };

class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	TYPE c_type = RECV;
	int _ai_target_obj;

	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		c_type = RECV;
		ZeroMemory(&_over, sizeof(_over));
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

class SESSION {
	OVER_EXP _recv_over;

public:
	SOCKET _socket;
	int _prev_size;	// 재조립에서 사용
	short _uid;		// 서버용 플레이어 고유 ID
	
	char _name[CHAR_SIZE];
	
	char direction;
	int _x, _y, _z;
	// character ability and skin ...
	// player item ...

	char _pet_num;
	// pet ability

	SESSION() {
		_socket = 0;
		_uid = -1;
		_x = _y = _z = -9999;
		direction = 1;
		strncpy_s(_name, "empty", strlen("empty"));
		_prev_size = 0;
		_pet_num = -1;
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_size;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_size;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}
};

array<SESSION, MAX_USER> clients;	// 플레이어's 컨테이너

class PARTY {
private:
	array<char[CHAR_SIZE], 4> member;
	array<char, 4> pets;

public:
	short get_member_count() {
		short count = 0;
		for (char* name : member) {
			if (0 != strcmp(name, "empty")) {
				count += 1;
			}
		}
		return count;
	}


};

void worker_thread(HANDLE h_iocp);
void process_packet(short c_uid, char* packet);