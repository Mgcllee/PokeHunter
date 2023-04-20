#pragma once

#include "DBModule.h"
#include "AWSModule.h"

std::array<SESSION, MAX_USER> clients;	// 플레이어's 컨테이너
std::array<PARTY, MAX_USER> parties;			// 총 파티 개수

void process_packet(int c_uid, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		// 이 실행문 전, AWS_Access Toekn이 필요함!, user Name이 없음
		CS_LOGIN_PACK* token_pack = reinterpret_cast<CS_LOGIN_PACK*>(packet);

		if (0 == strcmp(token_pack->Token, "theEnd")) {
			// std::string nameBuffer = GetPlayerName(clients[c_uid].IdToken);

			// AWS Cognito 에서 인증된 사용자만 입장

			if (true/*Login_UDB(c_uid, nameBuffer)*/) {
				{
					std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
					clients[c_uid]._state = ST_INGAME;
				}

				SC_LOGIN_SUCCESS_PACK ok_pack;
				ok_pack.size = sizeof(ok_pack);
				ok_pack.type = SC_LOGIN_SUCCESS;
				clients[c_uid].do_send(&ok_pack);
				std::cout << "Client[" << c_uid << "] user : Send Ok Packet!\n";

				SC_LOGIN_INFO_PACK info_pack;
				info_pack.size = sizeof(SC_LOGIN_INFO_PACK);
				info_pack.type = SC_LOGIN_INFO;
				strncpy_s(info_pack.name, CHAR_SIZE, clients[c_uid]._name, CHAR_SIZE);
				strncpy_s(info_pack._q_skill, CHAR_SIZE, clients[c_uid]._q_skill, CHAR_SIZE);
				info_pack._q_item = clients[c_uid]._q_item;
				info_pack._player_skin = clients[c_uid]._player_skin;
				info_pack._pet_num = clients[c_uid]._pet_num;
				clients[c_uid].do_send(&info_pack);
				
				std::cout << "Player Name: " << clients[c_uid]._name << " Login!\n";
			}
			else {
				SC_LOGIN_FAIL_PACK fail_pack;
				fail_pack.size = sizeof(SC_LOGIN_FAIL_PACK);
				fail_pack.type = SC_LOGIN_FAIL;
				clients[c_uid].do_send(&fail_pack);

				std::cout << "Login Fail!\n";
			}
		}
		else {
			std::string tokenBuffer;
			tokenBuffer.assign(token_pack->Token, (size_t)token_pack->Token_size);
			clients[c_uid].IdToken.append(tokenBuffer);
			std::cout << "Client ID: [" << c_uid << "] - " << clients[c_uid].IdToken << std::endl << std::endl;
			/*int value = WSAGetLastError();
			std::cout << "Len: " << (int)token_pack->Token_size << "\tError: " << value << std::endl;*/
		}
 	}
	break;
	case CS_QUEST_INVENTORY:
	{
		// DB에서 c_uid에 해당하는 아이템 정보 가져오기
		Get_ALL_ItemDB(c_uid);

		// 재사용할 아이템 패킷
		// 재사용시, Zeromemory로 초기화 필요한지 확인 필요.(데이터 오류 방지)
		SC_ITEM_INFO_PACK item_pack;
		item_pack.size = sizeof(SC_ITEM_INFO_PACK);
		item_pack.type = SC_ITEM_INFO;

		for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
			for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {
				std::string item_name = Get_ItemName(category, item_num);
				if (item_name.compare("NULL")) break;

				strncpy_s(item_pack._name, CHAR_SIZE, item_name.c_str(), strlen(item_name.c_str()));

				for (int cnt_num = 0; cnt_num < 9; ++cnt_num) {
					item_pack._cnt = clients[c_uid].get_item_arrayName(category)[cnt_num];
					clients[c_uid].do_send(&item_pack);
				}
			}
		}

		/*strncpy_s(item_pack._name, CHAR_SIZE, "bullet", strlen("bullet"));
		for (char& L : clients[c_uid].Launcher) {
			item_pack._cnt = L;
			clients[c_uid].do_send(&item_pack);
		}
		strncpy_s(item_pack._name, CHAR_SIZE, "bullet", strlen("bullet"));
		for (char& L : clients[c_uid].Install) {
			item_pack._cnt = L;
			clients[c_uid].do_send(&item_pack);
		}
		strncpy_s(item_pack._name, CHAR_SIZE, "firebullet", strlen("firebullet"));
		for (char& L : clients[c_uid].Potion) {
			item_pack._cnt = L;
			clients[c_uid].do_send(&item_pack);
		}
		strncpy_s(item_pack._name, CHAR_SIZE, "firebullet", strlen("firebullet"));
		for (char& L : clients[c_uid].Potion) {
			item_pack._cnt = L;
			clients[c_uid].do_send(&item_pack);
		}*/

		strncpy_s(item_pack._name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
		clients[c_uid].do_send(&item_pack);
		std::cout << "Send inventory data\n";
	}
	break;
	case CS_SAVE_INVENTORY:
	{
		CS_SAVE_INVENTORY_PACK* save_pack = reinterpret_cast<CS_SAVE_INVENTORY_PACK*>(packet);
		
		// DB 저장 끝
		if (0 == strcmp(save_pack->_name, "theEnd")) {
			save_pack->_name;
			save_pack->_cnt;
		}
		else {
			// DB에서 c_uid에 해당하는 아이템 정보 저장하기
			// Get_IDB(c_uid);
		}
	}
	break;
	case CS_PARTY_SEARCHING:
	{
		SC_PARTIES_INFO_PACK party_list;
		party_list.size = sizeof(SC_PARTIES_INFO_PACK);
		party_list.type = SC_PARTY_LIST_INFO;

		/*
		for (PARTY& pt : parties) {
			if (0 == strcmp(pt._name, "Empty") && 0 == pt._mem_count) 
				continue;

			strncpy_s(party_list._name, pt._name, strlen(pt._name));
			party_list._staff_count = pt._mem_count;
			clients[c_uid].do_send(&party_list);
		}
		*/

		// ====================== Test Room =========================
		strncpy_s(party_list._name, "임시방", strlen("임시방"));
		party_list._staff_count = 0;
		clients[c_uid].do_send(&party_list);
		strncpy_s(party_list._name, "HiBye", strlen("HiBye"));
		party_list._staff_count = 4;
		clients[c_uid].do_send(&party_list);
		strncpy_s(party_list._name, "초보환영", strlen("초보환영"));
		party_list._staff_count = 4;
		clients[c_uid].do_send(&party_list);
		//===========================================================

		strncpy_s(party_list._name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
		clients[c_uid].do_send(&party_list);
		std::cout << "Send Party List!\n";
	}
	break;
	case CS_PARTY_INFO:
	{
		// 플레이어가 파티룸에 입장, 동시에 파티에 참여
		CS_PARTY_INFO_PACK* party_info = reinterpret_cast<CS_PARTY_INFO_PACK*>(packet);
		clients[c_uid]._party_num = static_cast<int>(party_info->party_num);
		clients[c_uid]._player_state = ST_NOTREADY;

		SC_PARTY_INFO_PACK in_party;
		in_party.size = sizeof(SC_PARTY_INFO_PACK);
		in_party.type = SC_PARTY_INFO;

		for (SESSION& cl : parties[clients[c_uid]._party_num].member) {
			if (-1 == cl._uid) {
				/*strncpy_s(in_party._mem, CHAR_SIZE, clients[c_uid]._name, CHAR_SIZE);
				in_party._mem_pet = clients[c_uid]._pet_num;
				in_party._mem_state = clients[c_uid]._player_state;*/
				strncpy_s(in_party._mem, CHAR_SIZE, "가짜인형", CHAR_SIZE);
				in_party._mem_pet = 1;
				in_party._mem_state = ST_NOTREADY;
				clients[c_uid].do_send(&in_party);
				break;	// 마지막에 넣으므로 더 이상 반복할 필요가 없다.
			}
			else {
				strncpy_s(in_party._mem, CHAR_SIZE, cl._name, CHAR_SIZE);
				in_party._mem_pet = cl._pet_num;
				in_party._mem_state = cl._player_state;
				clients[c_uid].do_send(&in_party);
			}
		}

		// Test Data
		/*
		strncpy_s(in_party._mem, CHAR_SIZE, "명철", strlen("명철"));
		in_party._mem_pet = 3;
		in_party._mem_state = ST_NOTREADY;
		clients[c_uid].do_send(&in_party);

		strncpy_s(in_party._mem, CHAR_SIZE, "현석", strlen("현석"));
		in_party._mem_pet = 2;
		in_party._mem_state = ST_READY;
		clients[c_uid].do_send(&in_party);

		strncpy_s(in_party._mem, CHAR_SIZE, "희재", strlen("희재"));
		in_party._mem_pet = 1;
		in_party._mem_state = ST_NOTREADY;
		clients[c_uid].do_send(&in_party);
		*/

		strncpy_s(in_party._mem, CHAR_SIZE, "theEnd", strlen("theEnd"));
		clients[c_uid].do_send(&in_party);
		std::cout << "Send Party info\n";
	}
	break;
	case CS_PARTY_READY:	// 파티 시작 준비완료, client에서 플레이어가 READY BTN을 누를 때 마다 송/수신
	{
		char ready_member = 0;
		clients[c_uid]._player_state = ST_READY;	// 현재 내 플레이어는 READY로 설정

		for (SESSION& cl : parties[clients[c_uid]._party_num].member) {
			if (c_uid == cl._uid) continue;

			if (ST_READY == cl._player_state)
				ready_member += 1;
		}

		if (ready_member == parties[clients[c_uid]._party_num].member.size()) {	// 전원이 준비완료가 되어 출발 신호를 파티 내 모든 클라에게 송신
			SC_PARTY_JOIN_SUCCESS_PACK ok_pack;
			ok_pack.size = sizeof(SC_PARTY_JOIN_SUCCESS_PACK);
			ok_pack.type = SC_PARTY_JOIN_SUCCESS;

			for (SESSION& cl : parties[clients[c_uid]._party_num].member) {
				cl.do_send(&ok_pack);
			}
		}
		else {																	// 1명 이상의 플레이어가 준비되어 있지 않아 준비안됨 패킷을 송신
			SC_PARTY_JOIN_FAIL_PACK fail_pack;
			fail_pack.size = sizeof(SC_PARTY_JOIN_FAIL_PACK);
			fail_pack.size = SC_PARTY_JOIN_FAIL;

			for (SESSION& cl : parties[clients[c_uid]._party_num].member) {
				cl.do_send(&fail_pack);
			}
		}
	}
	break;
	case CS_PARTY_JOIN:		// 파티가 Survival Stage에 입장한다는 신호(서버에서 전송함), 그러나 현재 필요도가 낮은 패킷이 됨?
	{
		/*
		char staff_ready_num = 0;
		CS_PARTY_JOIN_PACK* join_pack = reinterpret_cast<CS_PARTY_JOIN_PACK*>(packet);
		
		for (SESSION& cl : parties[join_pack->party_num].member) {
			if (c_uid == cl._uid) continue;

		}

		if (parties[clients[c_uid]._party_num].member.size() == staff_ready_num) {
			SC_PARTY_JOIN_SUCCESS_PACK fail_pack;
			fail_pack.size = sizeof(SC_PARTY_JOIN_SUCCESS_PACK);
			fail_pack.type = SC_PARTY_JOIN_SUCCESS;
			for (SESSION& cl : parties[clients[c_uid]._party_num].member) {
				cl.do_send(&fail_pack);
			}
		}
		else {
			SC_PARTY_JOIN_FAIL_PACK fail_pack;
			fail_pack.size = sizeof(SC_PARTY_JOIN_FAIL_PACK);
			fail_pack.type = SC_PARTY_JOIN_FAIL;
			for (SESSION& cl : parties[clients[c_uid]._party_num].member) {
				cl.do_send(&fail_pack);
			}
		}
		*/
	}
	break;
	case CS_PARTY_LEAVE:
	{
		CS_PARTY_LEAVE_PACK* old_staff = reinterpret_cast<CS_PARTY_LEAVE_PACK*>(packet);

		if (/*checking data && save data*/ parties[old_staff->party_num].leave_member(old_staff->name)) {
			SC_PARTY_LEAVE_SUCCESS_PACK leave_pack;
			leave_pack.size = sizeof(SC_PARTY_LEAVE_SUCCESS_PACK);
			leave_pack.type = SC_PARTY_LEAVE_SUCCESS;
			clients[c_uid].do_send(&leave_pack);
		}
		else {
			SC_PARTY_LEAVE_FAIL_PACK fail_leave_pack;
			fail_leave_pack.size = sizeof(SC_PARTY_LEAVE_FAIL_PACK);
			fail_leave_pack.type = SC_PARTY_LEAVE_FAIL;
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
		// clients[c_uid]._recv_over.c_type = TYPE::LOGOUT;
	}
	break;
	}
}

void disconnect(int c_uid)
{
	for (SESSION& cl : clients) {
		{
			std::lock_guard<std::mutex> ll(cl._lock);
			if (ST_INGAME == cl._state) continue;
		}
		if (c_uid == cl._uid) {
			// logout logic
			continue;
		}

		// send logout packet
	}

	// client 정보 정리
	closesocket(clients[c_uid]._socket);
	std::lock_guard<std::mutex> ll(clients[c_uid]._lock);
	clients[c_uid]._state = ST_FREE;
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
				std::cout << "Accept error\n";
			}
			else {
				std::cout << "GQCS Error on client[" << key << "]\t";
				// disconnect client
			}
		}
		if ((0 == num_bytes) && (ex_over->c_type == RECV)) continue;

		switch (ex_over->c_type) {
		case ACCEPT:	// accept new client
		{
			int new_c_uid = get_player_uid();

			if (-1 != new_c_uid) { // 접속 성공, 정보 받기
				{
					std::lock_guard<std::mutex> ll{ clients[new_c_uid]._lock };
					clients[new_c_uid]._state = ST_ALLOC;
				}

				clients[new_c_uid]._socket = g_c_socket;

				CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket), h_iocp, new_c_uid, 0);

				clients[new_c_uid].do_recv();
				g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

				// Dummy Client 확인용
				std::cout << new_c_uid << "번 NEW PLAYER!\n";
				if (clients[new_c_uid]._recv_over.c_type == RECV) {
					std::cout << "STATE: RECV\n";
				}
			}
			else {					// 접속 실패
				std::cout << "connect fail\n";
				break;
			}

			ZeroMemory(&g_a_over._over, sizeof(g_a_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);
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
			delete ex_over;
			break;
		}
	}
}