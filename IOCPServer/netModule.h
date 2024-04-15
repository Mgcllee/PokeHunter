#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <map>
#include <list>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "stdafx.h"
#include "protocol.h"

enum TYPE { ACCEPT, RECV, SEND, LOGOUT };
enum SESSION_STATE { ST_FREE, ST_ALLOC };
enum PLAYER_STATE { ST_HOME, ST_NOTREADY, ST_READY, ST_STAGE };

class OVER_EXP {
public:
	WSAOVERLAPPED	_over;
	TYPE			c_type;
	WSABUF			_wsabuf;
	char			_send_buf[BUF_SIZE];

	OVER_EXP();
	OVER_EXP(char* packet);
	~OVER_EXP();
};
class SESSION {
public:
	SESSION();
	~SESSION();
	bool recycle_session();

	SESSION& operator=(const SESSION& ref);

	void set_socket(SOCKET new_socket);
	int get_prev_size();
	void set_prev_size(int in);
	void do_recv();
	void do_send(void* packet);
	void disconnect();

private:
	OVER_EXP _recv_over;
	SOCKET _socket;
	int _prev_size;
};
class PLAYER {
public:
	PLAYER();
	PLAYER(const PLAYER& rhs);
	~PLAYER();
	void recycle_player();

	inline void _lock() { ll.lock(); }
	inline void _unlock() { ll.unlock(); }

	void set_uid(int in) { _uid = in; }
	SESSION* get_session();
	std::string* get_token();

	PLAYER& operator=(const PLAYER& ref);

	bool operator== (const PLAYER& rhs) const;

	char* get_name();
	void set_name(const char* in);

	void set_item(const char* in_item_name, short index, char cnt);
	short get_item();
	short get_storage_item();
	void send_fail(const char* fail_reason = "NONE");
	void send_self_info(const char* success_message = "NONE");

private:
	mutable std::mutex ll;

	char _name[CHAR_SIZE];
	short _uid = -1;
	char _pet_num[CHAR_SIZE];
	char _player_skin;

	std::map<std::string, short> inventory_data;
	std::map<std::string, short> storage_data;

	char _party_num = -1;

	std::string IdToken;
	short IdTokenLenght;

	PLAYER_STATE _player_state;

	SESSION _session;
};
class PARTY {
public:
	PARTY();
	~PARTY();

	bool new_member(PLAYER& new_mem);
	bool leave_member(char* mem_name);
	bool get_party_in_stage();
	short get_mem_count();

	std::list<PLAYER> member;

private:
	char _name[CHAR_SIZE];
	short _mem_count = 0;
	bool _inStage;

	std::mutex ll;

};

extern OVER_EXP g_a_over;
extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

extern std::array<PLAYER, MAX_USER> clients;
extern std::array<PARTY, MAX_PARTY> parties;

void worker_thread(HANDLE h_iocp);
void process_packet(int c_uid, char* packet);
void disconnect(int c_uid);