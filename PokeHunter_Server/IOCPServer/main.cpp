﻿#pragma once

#include "netModule.h"
#include "DBModule.h"

// TODO: make sure clean code(one function one behavior)
OVER_EXP g_a_over;
SOCKET g_s_socket, g_c_socket;

int main() {
	HANDLE h_iocp;
	WSADATA WSAData;
	int err_code = WSAStartup(MAKEWORD(2, 2), &WSAData);

	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN server_addr;						
	memset(&server_addr, 0, sizeof(server_addr));	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;	// 이후 주소변경 필요

	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), h_iocp, 0, 0);
	
	g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_a_over.c_type = ACCEPT;

	AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

	std::vector<std::thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();

	// TODO: make log thread use all log
	// std::thread log_thread(PacketWorker);

	for (int i = 0; i < num_threads; ++i)
	{
		worker_threads.emplace_back(PacketWorker::worker_thread, h_iocp, PacketWorker());
	}

	USER_DB_MANAGER* db = new USER_DB_MANAGER("localhost", "root", "0000", "user_info_db");
	db->test_mysql_function();

	for (auto& th : worker_threads)
		th.join();
	closesocket(g_s_socket);
	WSACleanup();
}
