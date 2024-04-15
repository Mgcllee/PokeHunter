/*
[Refactoring]
	1. O.O.D
	2. update SESSION Design
*/

#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <map>

#include <list>

#include "stdafx.h"
#include "protocol.h"


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

extern OVER_EXP g_a_over;
extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

class SESSION {
public:
	SESSION();
	~SESSION();
	bool recycle_session();

	SESSION& operator=(SESSION& ref);

	void set_socket(SOCKET new_socket);
	int get_prev_size();
	void set_prev_size(int in);
	void do_recv();
	void do_send(void* packet);

private:
	OVER_EXP _recv_over;
	SOCKET _socket;
	int _prev_size;
};

class PLAYER {
public:
	PLAYER();
	~PLAYER();
	void recycle_player();

	void _lock();
	void _unlock();

	void set_uid(int in);
	SESSION* get_session();
	std::string* get_token();

	PLAYER& operator=(PLAYER& ref);

	bool operator== (const PLAYER& rhs) const;

	char* get_name();
	void set_name(const char* in);

	void set_item(const char* in_item_name, short index, char cnt);
	short get_item();
	short get_storage_item();
	void send_fail(const char* fail_reason = "NONE");
	void send_self_info(const char* success_message = "NONE");

private:
	char _name[CHAR_SIZE];
	short _uid = -1;
	char _pet_num[CHAR_SIZE];
	char _player_skin;

	std::map<std::string, short> inventory_data;
	std::map<std::string, short> storage_data;

	char _party_num = -1;

	std::string IdToken;
	short IdTokenLenght;

	// std::mutex ll;
	class dummy_mutex
	{
	public:
		void lock() {}
		void unlock() {}
	} ll;

	PLAYER_STATE _player_state;

	SESSION _session;
};
class PARTY;
extern std::array<PLAYER, MAX_USER> clients;
extern std::array<PARTY, MAX_PARTY> parties;


// Dummy Client용
extern int g_c_uid;


void worker_thread(HANDLE h_iocp);

void process_packet(int c_uid, char* packet);

void disconnect(int c_uid);