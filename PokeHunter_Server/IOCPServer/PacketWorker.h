#pragma once

#include "stdafx.h"
#include "netModule.h"

#include "Player.h"
#include "Party.h"

class PacketWorker
{
public:
	PacketWorker()
		: overlapped(nullptr)
	{ }
	~PacketWorker() { }

	void send_log(std::string log);

	void run_packet_worker_threads(HANDLE h_iocp);
	void worker_thread(HANDLE h_iocp);

	void accept_new_client();
	void recv_new_message(OverlappedExpansion* exoverlapped);

	int get_new_client_ticket();
	void set_new_client_ticket(int user_id);

	void process_packet(int, char*);

	bool check_exists_overlapped(OverlappedExpansion* overlapped);
	void sync_new_chatting_all_client(int user_id, std::string content);
	void get_party_list(int user_id);
	void disconnect(int user_id);

private:
	HANDLE iocp_handle;
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	BOOL GQCS_result;

	OverlappedExpansion accepter_overlapped;
	HANDLE handle_iocp;
	SOCKET server_socket, client_accept_socket;
	
	std::vector<Player> clients;
	std::vector<Party> parties;
};

