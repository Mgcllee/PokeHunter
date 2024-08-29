#pragma once

#include "stdafx.h"
#include "netModule.h"

class PacketWorker
{
public:
	PacketWorker()
		: overlapped(nullptr)
	{

	}
	~PacketWorker() { }

	void worker_thread(HANDLE h_iocp);
	void process_packet(int, char*);

	bool check_exists_overlapped(OverlappedExpansion* overlapped);

	void sync_new_chatting_all_client(int user_id, std::string content);

	void get_party_list(int user_id);
	void disconnect(int user_id);

	void accept_new_client();
	int get_new_client_ticket();
	void set_new_client_ticket(int user_id);

	void recv_new_message(OverlappedExpansion* exoverlapped);

	void send_log(std::string log);

private:
	HANDLE iocp_handle;
	DWORD num_bytes;
	ULONG_PTR key;
	WSAOVERLAPPED* overlapped;

	BOOL GQCS_result;
};

