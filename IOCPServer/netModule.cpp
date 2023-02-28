#pragma once

#include "netModule.h"
#include "DBModule.h"

void process_packet(short c_uid, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACK* p = reinterpret_cast<CS_LOGIN_PACK*>(packet);
		short new_c_uid = -1;

		// DB data checking
		if (/*DB*/true && new_c_uid != -1) {
			// Login Sucssess
			
			SC_LOGIN_SUCCESS_PACK ok_pack;
			ok_pack.size = sizeof(ok_pack);
			ok_pack.type = SC_LOGIN_SUCCESS;
			clients[new_c_uid].do_send(&ok_pack);

			SC_LOGIN_INFO_PACK info_pack;
			info_pack.size = sizeof(SC_LOGIN_INFO_PACK);
			info_pack.type = SC_LOGIN_INFO;

			// 새로 접속한 클라이언트 정보를 다른 기존 클라이언트들에게 전송
			for (SESSION& c : clients) {
				c.do_send(&info_pack);
			}

			SC_LOGIN_INFO_PACK old_info_pack;
			old_info_pack.size = sizeof(SC_LOGIN_INFO_PACK);
			old_info_pack.type = SC_LOGIN_INFO;

			// 새로운 클라이언트에게 기존 클라이언트들 정보를 전부 전송
			for (SESSION& c : clients) {
				// 새로운 클라이언트에게 자기 자신의 정보는 보낼 필요 없음(위 반복문과 중복됨)
				if (new_c_uid != c._uid) {
					old_info_pack.name;
					old_info_pack.x = c._x;
					old_info_pack.y = c._y;
					old_info_pack.z = c._z;
					strncpy_s(old_info_pack.name, c._name, CHAR_SIZE);

					// 새로운 클라이언트에게 전송
					clients[new_c_uid].do_send(&old_info_pack);
				}
			}
		}
		else if(new_c_uid != -1){
			SC_LOGIN_FAIL_PACK fail_pack;
			fail_pack.size = sizeof(SC_LOGIN_FAIL_PACK);
			fail_pack.type = SC_LOGIN_FAIL;
			clients[new_c_uid].do_send(&fail_pack);
		}
	}
	break;
	case CS_SEARCHING_PARTY:
	{
		//send party list
		SC_PARTY_LIST_INFO_PACK party_list;
		party_list.size = sizeof(SC_PARTY_LIST_INFO_PACK);
		party_list.type = SC_PARTY_LIST_INFO;
		strcpy_s(party_list._name, "TEST PARTY");
		party_list._staff_member = 1;	// char형 주의

		clients[c_uid].do_send(&party_list);
	}
	break;
	case CS_PARTY_INFO:
	{
		CS_PARTY_INFO_PACK* party_info = reinterpret_cast<CS_PARTY_INFO_PACK*>(packet);

		char party_num = party_info->party_num;

		// get partys[party_num] info

	}
	break;
	case CS_LOGOUT:
	{
		//Disconnect client
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