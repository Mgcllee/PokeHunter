#pragma once

#include "netModule.h"
#include "DBModule.h"

void process_packet(short c_uid, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACK* p = reinterpret_cast<CS_LOGIN_PACK*>(packet);
		
		// DB data checking
		if (/*DB*/true) {

		}
		else {

		}
	}
	break;
	case CS_MOVE:
	{

	}
	break;
	case CS_ATTACK:
	{

	}
	break;
	case CS_LOGOUT:
	{

	}
	break;
	}
}

void worker_thread(HANDLE h_iocp)
{
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);

		// error 검출기
		if (FALSE == ret) {
			if (ex_over->c_type == ACCEPT) {
				std::string err_text = "errorCollecter.txt";
				std::ofstream err_file(err_text.data());

				if (err_file.is_open()) {
					err_file << "";

					time_t timer;
					struct tm* t;

					timer = time(NULL);
					t = localtime(&timer);

					err_file << t->tm_year + 1900 << "/" << t->tm_mon + 1 << "/" << t->tm_mday
						<< "-(" << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << ")"
						<< " : Accept Error\n";
				}
			}
			else continue;
		}
		if ((0 == num_bytes) && (ex_over->c_type == RECV)) continue;

		switch (ex_over->c_type) {
		case ACCEPT:	// accept new client
		{
			// newClient 고유번호 부여, 게임정보 입력, 연결
			short new_c_uid = get_player_uid();
			
			if (-1 != new_c_uid) { // 접속 성공, 정보 받기
				clients[new_c_uid]._x;
				clients[new_c_uid]._y;
				clients[new_c_uid]._z;
				clients[new_c_uid].direction;

				clients[new_c_uid]._uid = new_c_uid;
				clients[new_c_uid]._socket = g_c_socket;

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket), h_iocp, new_c_uid, 0);
				clients[new_c_uid].do_recv();
				g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

			}
			else {					// 접속 실패

			}

			
		}
		break;
		case RECV:		// get new message
		{
			// 패킷 재조립
		}
		break;
		case SEND:		// send new message
		{

		}
		break;
		}
	}
}