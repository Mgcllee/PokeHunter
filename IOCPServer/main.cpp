#pragma once
#include "stdafx.h"
#include "netModule.h"

OVER_EXP g_a_over;

int main() {
	WSADATA WSAData;
	int err_code = WSAStartup(MAKEWORD(2, 2), &WSAData);

	g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN server_addr;							// ���� �⺻ ����ü
	memset(&server_addr, 0, sizeof(server_addr));		// ����ü ����� ���� �ʱ�ȭ
	server_addr.sin_family = AF_INET;					// IPv4�� �����
	server_addr.sin_port = htons(PORT_NUM);				// ���α׷��� ��Ʈ��ȣ�� PORT_NUM
	// server_addr.sin_addr.S_un.S_addr = INADDR_ANY;	// ���� ��ī�带 ����� ��� ���

	bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(g_s_socket, SOMAXCONN);	// �ִ� ���� Ŭ��� (SOMAXCONN)

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), h_iocp, 9999, 0);
	
	g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_a_over.c_type = ACCEPT;

	err_code = AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

	vector<thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();	// PC�� ����ھ� ����

	// === WORK SPACE ===
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back(worker_thread, h_iocp);
	// === ========== ===

	// ���α׷� ���� ����
	for (auto& th : worker_threads)
		th.join();
	closesocket(g_s_socket);
	WSACleanup();
}
