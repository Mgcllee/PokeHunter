#pragma once

#include "stdafx.h"
#include "protocol.h"

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

//#include <aws/core/aws.h>
//#include <aws/lambda/model/Runtime.h>
//using namespace Aws::Lambda::Model;
//const Aws::String;	// get userName

enum TYPE { ACCEPT, RECV, SEND };
enum CLIENT_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
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

class SESSION {
	OVER_EXP _recv_over;

	char _name[CHAR_SIZE];
public:
	SOCKET _socket;
	int _prev_size;	// 재조립에서 사용

	short _uid;		// 서버용 플레이어 고유 ID	
	char _pet_num;
	char _player_skin;

	char Collection[9];
	char Install[9];
	char Launcher[9];
	char Potion[9];

	char _q_item;
	char _q_skill[4];

	char _party_num;		// 파티 고유 번호
	char _party_staff_num;	// 파티 내 멤버 번호

	CLIENT_STATE _state;
	std::mutex _lock;

	PLAYER_STATE _player_state;

	SESSION() {
		_socket = 0;
		_uid = -1;
		strncpy_s(_name, "empty", strlen("empty"));
		_prev_size = 0;
		_pet_num = -1;
		_state = ST_FREE;
	}

	SESSION& operator=(SESSION& ref) {
		strncpy_s(this->_name, ref.get_name(), strlen(ref.get_name()));
		this->_socket = ref._socket;
		this->_prev_size = ref._prev_size;

		this->_uid = ref._uid;
		this->_pet_num = ref._pet_num;
		this->_player_skin = ref._player_skin;

		strncpy_s(this->Collection, ref.Collection, strlen(ref.Collection));
		strncpy_s(this->Install, ref.Install, strlen(ref.Install));
		strncpy_s(this->Launcher, ref.Launcher, strlen(ref.Launcher));
		strncpy_s(this->Potion, ref.Potion, strlen(ref.Potion));

		this->_q_item = ref._q_item;
		strncpy_s(this->_q_skill, ref._q_skill, strlen(ref._q_skill));

		this->_state = ref._state;
		this->_player_state = ref._player_state;
	}

	void clear() {
		_socket = 0;
		_uid = -1;
		strncpy_s(_name, "empty", strlen("empty"));
		_prev_size = 0;
		_pet_num = -1;
		_state = ST_FREE;
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_size;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_size;
		_recv_over.c_type = RECV;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* send_over = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &send_over->_wsabuf, 1, 0, 0, &send_over->_over, 0);
	}

	char* get_name() {
		return _name;
	}
	void set_name(const char* in) {
		strncpy_s(_name, CHAR_SIZE, in, sizeof(in));
	}

	void set_item(const char* in_item_name, short index, char cnt) {
		if (0 == strcmp(in_item_name, "CT")) {
			Collection[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "IS")) {
			Install[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "LC")) {
			Launcher[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "PT")) {
			Potion[index] = cnt;
		}
	}

	char* get_item_arrayName(short num)
	{
		switch(num)
		{
		case 1:
			return Collection;
			break;
		case 2:
			return Install;
			break;
		case 3:
			return Launcher;
			break;
		case 4:
			return Potion;
			break;
		}
	}
};

class PARTY {
private:
	char party_name[CHAR_SIZE];
	short mem_count{};

public:
	PARTY() {
		
	}
	~PARTY() {
		
	}

	std::array<SESSION, 4> member;

	short get_member_count() {
		return mem_count;
	}

	bool new_member(SESSION& new_mem) {
		if (mem_count >= 4) return false;

		for (SESSION& mem : member) {
			if (0 == strcmp(mem.get_name(), "empty")) {
				mem = new_mem;
				mem_count += 1;
				return true;
			}
		}
		
		// leave_member()가 작동오류를 했거나, 초기화가 문제임
		return false;
	}

	bool leave_member(char* mem_name) {
		if (mem_count <= 0) return false;

		for (SESSION& mem : member) {
			if (0 == strcmp(mem.get_name(), mem_name)) {
				mem.clear();
				mem_count -= 1;
				return true;
			}
		}

		// 탐색된 파티원 중 입력된 파티원 정보 없음
		return false;
	}

	void get_party_info(std::array<char[CHAR_SIZE], 4>& in_member, std::array<char, 4>& in_pet) {
		for (int i = 0; i < 4; ++i) {
			if (0 != strcmp(member[i].get_name(), "emtpy")) {
				strncpy_s(in_member[i], member[i].get_name(), sizeof(member[i]).get_name());
				in_pet[i] = member[i]._pet_num;
			}
		}
	}
};

extern OVER_EXP g_a_over;
extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

extern std::array<SESSION, MAX_USER> clients;	// 플레이어's 컨테이너
extern std::array<PARTY, MAX_PARTY> partys;

void worker_thread(HANDLE h_iocp);
void process_packet(short c_uid, char* packet);

void disconnect(short c_uid);