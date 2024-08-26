#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <unordered_map>
#include <list>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include "stdafx.h"
#include "protocol.h"

enum SOCKET_TYPE	{ ACCEPT, RECV, SEND, LOGOUT };
enum SESSION_TYPE	{ FREE, ALLOC };
enum PLAYER_STATE	{ HOME, NOT_READY, READY, STAGE };

class OverlappedExpansion {
public:
	OverlappedExpansion();
	OverlappedExpansion(char* packet);
	~OverlappedExpansion();

private:

public:
	WSAOVERLAPPED	overlapped;
	SOCKET_TYPE		socket_type;
	WSABUF			wsa_buffer;
	char			packet_buffer[BUF_SIZE];

private:

};

class Session : public OverlappedExpansion {
public:
	Session();
	~Session();

	Session& operator=(const Session& ref);

	void set_socket(SOCKET new_socket);
	void recv_packet();
	void send_packet(void* packet);

	void send_all_client(void* packet);
	
	int get_prev_remain_packet_size();
	void set_curr_remain_packet_size(int in);
	
	void disconnect();

	bool recycle_session();

private:

public:

private:
	SOCKET socket;
	int remain_packet_size;
};

class Player {
public:
	Player();
	Player(const Player& rhs);
	~Player();

	Player& operator=(const Player& ref);
	bool operator== (const Player& rhs) const;

	void lock() { ll.lock(); }
	void unlock() { ll.unlock(); }

	void set_user_id(int new_user_id) { user_id = new_user_id; }
	Session* get_session();
	std::string* get_cognito_id_token();

	char* get_user_name();
	void set_user_name(const char* in);

	void set_item(const char* in_item_name, short index, char cnt);
	short get_item();
	short get_storage_item();
	void send_fail_reason(const char* fail_reason = "NONE");
	void send_self_user_info(const char* success_message = "NONE");

	void recycle_player();

	void check_exists_token(char*);

	void get_all_inventory_item();
	bool set_inventory_item(char* item_name, char item_count);

private:

public:

private:
	mutable std::mutex ll;

	char name[CHAR_SIZE];
	short user_id;
	char pet_type[CHAR_SIZE];
	char player_skin_type;

	std::unordered_map<std::string, short> inventory_data;
	std::unordered_map<std::string, short> storage_data;

	char party_num;

	std::string cognito_id_token;

	PLAYER_STATE player_state;

	Session session;
};

class Party {
public:
	Party();
	~Party();

	bool new_member(Player& new_mem);
	bool leave_member(char* mem_name);
	bool get_party_in_stage();
	short get_member_count();

	std::list<Player> member;

private:
	char name[CHAR_SIZE];
	short member_count;
	bool in_stage;

	std::mutex ll;
};

class PacketWorker {
	/*
	1. packet convert
	2. process packet content
	3. sync all client
	*/

public:

	void process_packet(int, char*);
	void sync_new_chatting_all_client(int user_id, std::string content);
};

extern OverlappedExpansion glbal_overlapped;
extern HANDLE handle_iocp;
extern SOCKET global_server_socket, global_client_accept_socket;

extern std::array<Player, MAX_USER> clients;
extern std::array<Party, MAX_PARTY> parties;

void worker_thread(HANDLE h_iocp);
void process_packet(int c_uid, char* packet);
void disconnect(int c_uid);