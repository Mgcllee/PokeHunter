#pragma once

#include "DBModule.h"

array<SESSION, MAX_USER> clients;	// 플레이어's 컨테이너
array<PARTY, MAX_PARTY> partys;

void process_packet(short c_uid, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACK* p = reinterpret_cast<CS_LOGIN_PACK*>(packet);
		short new_c_uid = -1;
		char db_name[CHAR_SIZE], db_skill[4];
		char db_skin, db_pet, db_item;

		if (Login_UDB(p->id, p->pw, new_c_uid, db_name, db_skin, db_pet, db_item, db_skill) && new_c_uid != -1) {
			clients[c_uid].set_name(db_name);
			
			SC_LOGIN_SUCCESS_PACK ok_pack;
			ok_pack.size = sizeof(ok_pack);
			ok_pack.type = SC_LOGIN_SUCCESS;
			clients[new_c_uid].do_send(&ok_pack);
			
			SC_LOGIN_INFO_PACK info_pack;
			info_pack.size = sizeof(SC_LOGIN_INFO_PACK);
			info_pack.type = SC_LOGIN_INFO;
			strncpy_s(info_pack.name, sizeof(info_pack.name), clients[c_uid].get_name(), sizeof(clients[c_uid].get_name()));
			info_pack._player_skin = db_skin;
			info_pack._pet_num = db_pet;
			info_pack.q_item = db_item;
			strncpy_s(info_pack.q_skill, sizeof(info_pack.q_skill), db_skill, sizeof(db_skill));
		
			// 새로 접속한 클라이언트 정보를 다른 기존 클라이언트들에게 전송
			for (SESSION& c : clients) {
				c.do_send(&info_pack);
			}

			/*
			SC_LOGIN_INFO_PACK old_info_pack;
			old_info_pack.size = sizeof(SC_LOGIN_INFO_PACK);
			old_info_pack.type = SC_LOGIN_INFO;

			// 새로운 클라이언트에게 기존 클라이언트들 정보를 전부 전송
			for (SESSION& c : clients) {
				// 새로운 클라이언트에게 자기 자신의 정보는 보낼 필요 없음(위 반복문과 중복됨)
				if (new_c_uid != c._uid) {
					old_info_pack.name;
					// strncpy_s(old_info_pack.name, c._name, CHAR_SIZE);

					// 새로운 클라이언트에게 전송
					clients[new_c_uid].do_send(&old_info_pack);
				}
			}

			cout << "Login Success! Name: " << clients[c_uid].get_name() << endl << "Skin: " << info_pack._player_skin << "		Pet: " << info_pack._pet_num << 
				"	Item: " << info_pack.q_item << "	Skill: " << info_pack.q_skill << endl;
			*/

			cout << "Name: " << db_name << "\nPlayer Skin: " << db_skin << "\nPlayer Pet: " << db_pet << "\nItem: " << db_item << "		Skill: " << db_skill << endl;
		}
		else {
			SC_LOGIN_FAIL_PACK fail_pack;
			fail_pack.size = sizeof(SC_LOGIN_FAIL_PACK);
			fail_pack.type = SC_LOGIN_FAIL;
			clients[new_c_uid].do_send(&fail_pack);

			cout << "Login Fail!" << endl;
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
	case CS_PARTY_INFO:	// 플레이어가 파티룸에 입장
	{
		CS_PARTY_INFO_PACK* party_info = reinterpret_cast<CS_PARTY_INFO_PACK*>(packet);

		short party_num = static_cast<int>(party_info->party_num);
		
		array<char[CHAR_SIZE], 4> member;
		array<char, 4> pets;
		partys[party_num].get_party_info(member, pets);

		// get partys[party_num] info
		SC_PARTY_INFO_PACK in_party;
		in_party.size = sizeof(SC_PARTY_INFO_PACK);
		in_party.type = SC_PARTY_INFO;

		if (0 <= party_num) {
			short staff_member = partys[party_num].get_member_count();
			for (int i = 0; i < staff_member; ++i) {
				strncpy_s(in_party._mem[i], sizeof(in_party._mem[i]), member[i], sizeof(member[i]));
				in_party._mem_pet[i] = pets[i];
			}
		}

		clients[c_uid].do_send(&in_party);
	}
	break;
	case CS_JOIN_PARTY:
	{
		CS_JOIN_PARTY_PACK* new_staff = reinterpret_cast<CS_JOIN_PARTY_PACK*>(packet);

		char* staff_name{};
		char staff_pet = static_cast<char>(1);
		strncpy_s(staff_name, sizeof(staff_name), "empty", sizeof("empty"));

		partys[new_staff->party_num].new_member(staff_name, staff_pet);
	}
	break;
	case CS_LEAVE_PARTY:
	{
		CS_LEAVE_PARTY_PACK* old_staff = reinterpret_cast<CS_LEAVE_PARTY_PACK*>(packet);

		// checking_DB(old_staff->name, c_uid);
		
		if (/*checking data && save data*/ partys[old_staff->party_num].out_party_staff(old_staff->name)) {
			SC_LEAVE_PARTY_SUCCESS_PACK leave_pack;
			leave_pack.size = sizeof(SC_LEAVE_PARTY_SUCCESS_PACK);
			leave_pack.type = SC_LEAVE_PARTY_SUCCESS;
			clients[c_uid].do_send(&leave_pack);
		}
		else {
			SC_LEAVE_PARTY_FAIL_PACK fail_leave_pack;
			fail_leave_pack.size = sizeof(SC_LEAVE_PARTY_FAIL_PACK);
			fail_leave_pack.type = SC_LEAVE_PARTY_FAIL;
			clients[c_uid].do_send(&fail_leave_pack);
		}
	}
	break;
	case CS_LOGOUT:
	{
		// Disconnect client
		CS_LOGOUT_PACK* logout_client = reinterpret_cast<CS_LOGOUT_PACK*>(packet);

		if (/*checking_DB(logout_client->name, c_uid)*/false) {
			SC_LOGOUT_SUCCESS_PACK out_client;
			out_client.size = sizeof(SC_LOGOUT_SUCCESS_PACK);
			out_client.type = SC_LOGOUT_SUCCESS;
			clients[c_uid].do_send(&out_client);
		}
		else {
			SC_LOGOUT_FAIL_PACK return_client;
			return_client.size = sizeof(SC_LOGOUT_FAIL_PACK);
			return_client.type = SC_LOGOUT_FAIL;
			clients[c_uid].do_send(&return_client);
		}
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
				cout << "Accept error\n";
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
				clients[new_c_uid]._socket = g_c_socket;

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket), h_iocp, NULL, 0);
				clients[new_c_uid].do_recv();
				g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

				ZeroMemory(&g_a_over._over, sizeof(g_a_over._over));
				int addr_size = sizeof(SOCKADDR_IN);
				AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

				// Dummy Client 확인용
				cout << "NEW PLAYER!" << endl;
			}
			else {					// 접속 실패
				cout << "connect fail\n";
				break;
			}
		}
		break;
		case RECV:		// get new message
		{
			// 패킷 재조립
			int remain_data = num_bytes + clients[key]._prev_size;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_size = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
		}
		break;
		case SEND:		// send new message
		{

		}
		break;
		}
	}
}