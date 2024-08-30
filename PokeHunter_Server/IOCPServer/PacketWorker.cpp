#pragma once

#include "PacketWorker.h"

void PacketWorker::worker_thread(HANDLE h_iocp)
{
	iocp_handle = h_iocp;

	while (true)
	{
		GQCS_result = GetQueuedCompletionStatus(iocp_handle, &num_bytes, &key, &overlapped, INFINITE);
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(overlapped);

		if (check_exists_overlapped(exoverlapped))
		{
			continue;
		}

		switch (exoverlapped->socket_type)
		{
		case SOCKET_TYPE::ACCEPT:
			accept_new_client();
			break;

		case SOCKET_TYPE::RECV:
			recv_new_message(exoverlapped);
			break;

		case SOCKET_TYPE::SEND:
			delete exoverlapped;
			break;

		default:
			send_log("[Process Function]: Wrong SOCKET TYPE " + std::to_string(key));
			break;
		}
	}
}

void PacketWorker::recv_new_message(OverlappedExpansion* exoverlapped)
{
	int remain_data = num_bytes + clients[key].get_session()->get_prev_remain_packet_size();
	char* p = exoverlapped->packet_buffer;

	while (remain_data > 0) {
		int packet_size = p[0];
		if (packet_size <= remain_data) {
			process_packet(static_cast<int>(key), p);
			p = p + packet_size;
			remain_data = remain_data - packet_size;
		}
		else break;
	}

	clients[key].get_session()->set_curr_remain_packet_size(remain_data);

	if (remain_data > 0) {
		memcpy(exoverlapped->packet_buffer, p, remain_data);
	}
	clients[key].get_session()->recv_packet();
}

void PacketWorker::process_packet(int user_id, char* packet)
{
	switch (packet[1])
	{
	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_LOGIN:
		clients[user_id].check_exists_token(reinterpret_cast<CS_LOGIN_PACK*>(packet)->Token);
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_CHAT:
		sync_new_chatting_all_client(user_id, reinterpret_cast<CS_CHAT_PACK*>(packet)->content);
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_QUEST_INVENTORY:
		clients[user_id].get_all_inventory_item();
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_SAVE_INVENTORY:
		clients[user_id].set_inventory_item
		(
			reinterpret_cast<CS_SAVE_INVENTORY_PACK*>(packet)->_name, 
			reinterpret_cast<CS_SAVE_INVENTORY_PACK*>(packet)->_cnt
		);
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_QUEST_STORAGE:
		clients[user_id].get_all_storage_item();
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_SEARCHING:
		get_party_list(user_id);
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_ENTER:
		if (int party_number = reinterpret_cast<CS_PARTY_ENTER_PACK*>(packet)->party_num)
		{
			parties[party_number].enter_member(clients[user_id]);
		}
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_READY:
		clients[user_id].set_ready_in_party();
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_PARTY_LEAVE:
		clients[user_id].leave_current_party();
		break;

	case SEND_CLIENT_TO_SERVER_PACKET_TYPE::CS_LOGOUT:
		clients[user_id].recycle_player();
		break;
	}
}

bool PacketWorker::check_exists_overlapped(OverlappedExpansion* exoverlapped)
{
	if (FALSE == GQCS_result) {
		if (exoverlapped->socket_type == ACCEPT)
		{
			send_log("Accept error\n");
		}
		else
		{
			send_log("GQCS Error on client[" + std::to_string(key) + "]\n");
			disconnect(key);
		}
	}
	if ((0 == num_bytes) && (exoverlapped->socket_type == RECV)) return false;

	return true;
}

void PacketWorker::sync_new_chatting_all_client(int user_id, std::string content)
{
	std::string chat_log = std::string("[chat][").append(clients[user_id].get_user_name()).append("]: ").append(content);

	SC_CHAT_PACK ctp;
	ctp.size = sizeof(SC_CHAT_PACK);
	ctp.type = SC_CHAT;
	strcpy_s(ctp.content, chat_log.c_str());

	for (Player& p : clients)
	{
		p.get_session()->send_packet(&ctp);
	}
}

void PacketWorker::get_party_list(int user_id)
{
	SC_PARTIES_INFO_PACK party_list;
	party_list.size = sizeof(SC_PARTIES_INFO_PACK);
	party_list.type = SC_PARTY_LIST_INFO;

	for (int i = 0; i < MAX_PARTY; ++i) {
		party_list._staff_count[i] = static_cast<char>(parties[i].get_member_count());

		if (parties[i].get_party_in_stage())
			party_list.Inaccessible[i] = 1;
		else
			party_list.Inaccessible[i] = 0;
	}

	clients[user_id].get_session()->send_packet(&party_list);
}

void PacketWorker::disconnect(int user_id)
{
	clients[user_id].get_session()->~Session();
}

void PacketWorker::accept_new_client()
{
	get_new_client_ticket();

	ZeroMemory(&accepter_overlapped.overlapped, sizeof(accepter_overlapped.overlapped));
	int addr_size = sizeof(SOCKADDR_IN);
	AcceptEx(
		server_socket, client_accept_socket,
		accepter_overlapped.packet_buffer, 0, addr_size + 16, addr_size + 16, 0,
		&accepter_overlapped.overlapped
	);
}

int PacketWorker::get_new_client_ticket()
{
	// TODO: make new client ticket 
	int ticket = -1;

	set_new_client_ticket(ticket);
	return ticket;
}

void PacketWorker::set_new_client_ticket(int user_id)
{
	int new_user_id = get_new_client_ticket();

	if (-1 != new_user_id)
	{
		clients[new_user_id].lock();

		clients[new_user_id].get_session()->set_socket(server_socket);
		clients[new_user_id].set_user_id(new_user_id);
		clients[new_user_id].unlock();

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_accept_socket), iocp_handle, new_user_id, 0);

		clients[new_user_id].get_session()->recv_packet();
		client_accept_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}
	else
	{
		return;
	}
}

void PacketWorker::run_packet_worker_threads(HANDLE h_iocp)
{
	std::vector<std::thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();

	// TODO: make log thread use all log
	
	// TODO: USER_DB_MANAGER class run on db_thread
	
	for (int i = 0; i < num_threads; ++i)
	{
		worker_threads.emplace_back(&PacketWorker::worker_thread, new PacketWorker, h_iocp);
	}

	for (auto& each_thread : worker_threads)
	{
		each_thread.join();
	}
}
