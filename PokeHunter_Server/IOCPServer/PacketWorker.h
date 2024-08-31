#pragma once

#include "stdafx.h"
#include "netModule.h"

#include "PartiesManager.h"
#include "PlayerManager.h"

class PacketWorker
{
public:
	PacketWorker() { };
	PacketWorker(PlayerManager* player_manager, PartiesManager* parties_manager)
		: overlapped(nullptr)
		, players(player_manager)
		, parties(parties_manager)
	{ }
	~PacketWorker() { }

	void send_log(std::string log);

	void run_packet_worker_threads(HANDLE h_iocp);
	void worker_thread(HANDLE h_iocp);

	void accept_new_client();
	void recv_new_message(OverlappedExpansion* exoverlapped);

	int get_new_client_ticket();
	void set_new_client_ticket(int player_ticket);

	void process_packet(int, char*);

	bool check_exists_overlapped(OverlappedExpansion* overlapped);
	void sync_new_chatting_all_client(int user_id, char* packet);
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
	
	PlayerManager* players;
	PartiesManager* parties;
};

