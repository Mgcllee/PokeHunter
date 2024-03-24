#pragma once

#include "DBModule.h"
#include "AWSModule.h"

std::array<PLAYER, MAX_USER> clients;
std::array<PARTY, MAX_USER> parties;

class OVER_EXP {
public:
	WSAOVERLAPPED	_over;
	TYPE			c_type;
	WSABUF			_wsabuf;
	char			_send_buf[BUF_SIZE];

	OVER_EXP()
	{
		ZeroMemory(&_over, sizeof(_over));
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		c_type = RECV;
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		c_type = SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

class SESSION {
public:
	SESSION() :
		_recv_over(NULL)
		, _socket(NULL)
		, _prev_size(0)
		, IdToken(NULL)
		, IdTokenLenght(0)
		, _state(ST_FREE)
		, _lock()
	{

	}
	~SESSION()
	{

	}

	SESSION& operator=(SESSION& ref) {
		this->_socket = ref._socket;
		this->_prev_size = ref._prev_size;
		return *this;
	}

	bool recycle_session() {

		return false;
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_size;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_size;
		_recv_over.c_type = RECV;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
	}
	void do_send(void* packet)
	{
		OVER_EXP* send_over = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &send_over->_wsabuf, 1, 0, 0, &send_over->_over, 0);
	}
private:
	OVER_EXP _recv_over;
	SOCKET _socket;
	int _prev_size;

	std::string IdToken;
	short IdTokenLenght;

	CLIENT_STATE _state;
	std::mutex _lock;

};

class PLAYER {
public:
	char _name[CHAR_SIZE];
	short _uid = -1;
	char _pet_num[CHAR_SIZE];
	char _player_skin;

	char Collection[9];
	char Install[9];
	char Launcher[9];
	char Potion[9];

	std::map<std::string, short> itemData;

	char storageLauncher[9];
	char storageInstall[9];
	char storagePotion[9];
	char storageCollection[9];

	char _q_item;
	char _q_skill[CHAR_SIZE];

	char _party_num = -1;
	char _party_staff_num;

	std::string IdToken;
	short IdTokenLenght;

	CLIENT_STATE _state;
	std::mutex _lock;

	PLAYER_STATE _player_state;

	PLAYER() {
		_uid = -1;
		strncpy_s(_name, "None", strlen("None"));
		strncpy_s(_pet_num, "NonePartner", strlen("NonePartner"));
		_state = ST_FREE;
		itemData.clear();
	}
	// 사용 전 객체 재사용 가능한지 확인
	~PLAYER() {

	}

	PLAYER& operator=(PLAYER& ref) {
		strncpy_s(this->_name, ref.get_name(), strlen(ref.get_name()));

		this->_uid = ref._uid;
		strncpy_s(this->_pet_num, ref._pet_num, strlen(ref._pet_num));
		this->_player_skin = ref._player_skin;

		strncpy_s(this->Collection, ref.Collection, strlen(ref.Collection));
		strncpy_s(this->Install, ref.Install, strlen(ref.Install));
		strncpy_s(this->Launcher, ref.Launcher, strlen(ref.Launcher));
		strncpy_s(this->Potion, ref.Potion, strlen(ref.Potion));

		this->_q_item = ref._q_item;
		strncpy_s(this->_q_skill, ref._q_skill, strlen(ref._q_skill));

		this->_state = ref._state;
		this->_player_state = ref._player_state;

		return *this;
	}

	// 객체 재사용을 위한 함수
	void clear() {

	}

	char* get_name() {
		return _name;
	}

	void set_name(const char* in) {
		strncpy_s(_name, CHAR_SIZE, in, sizeof(in));
	}

	void set_item(const char* in_item_name, short index, char cnt) {
		if (0 == strcmp(in_item_name, "CT")) {
			Collection[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "IS")) {
			Install[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "LC")) {
			Launcher[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "PT")) {
			Potion[index] = cnt;
		}
	}

	char* get_item_arrayName(short num)
	{
		switch (num)
		{
		case 0:
			return Collection;
			break;
		case 1:
			return Install;
			break;
		case 2:
			return Launcher;
			break;
		case 3:
			return Potion;
			break;
		}
		return nullptr;
	}

	char* get_storage_item_arrayName(short num)
	{
		switch (num)
		{
		case 0:
			return storageCollection;
			break;
		case 1:
			return storageInstall;
			break;
		case 2:
			return storageLauncher;
			break;
		case 3:
			return storagePotion;
			break;
		}
		return nullptr;
	}
};

class PARTY {
public:
	PARTY() {
		strncpy_s(_name, CHAR_SIZE, "Empty", strlen("Empty"));
		_mem_count = 0;
		for (PLAYER& cl : member) {
			cl._uid = -1;
			strncpy_s(cl._name, CHAR_SIZE, "None", strlen("None"));
		}
		_inStage = false;
	}
	~PARTY() {

	}

	short get_member_count() {
		return _mem_count;
	}

	bool new_member(PLAYER& new_mem) {
		if (_mem_count >= 4) return false;

		for (PLAYER& mem : member) {
			if (0 == strcmp(mem.get_name(), "None")) {
				mem = new_mem;
				_mem_count += 1;
				return true;
			}
		}

		return false;
	}

	bool leave_member(char* mem_name) {
		if (_mem_count <= 0) return false;
		for (int i = 0; i < 4; ++i) {
			if (0 == strcmp(member[i].get_name(), mem_name)) {
				for (int j = i; j < 3; ++j) {
					{
						std::lock_guard<std::mutex> ll{ member[j]._lock };
						member[j] = member[j + 1];
					}
					member[member.size() - 1].clear();
				}

				_mem_count -= 1;
				if (0 == _mem_count) {
					_inStage = false;
				}

				return true;
			}
		}
		return false;
	}

private:
	char _name[CHAR_SIZE];
	short _mem_count = 0;
	bool _inStage;

	std::array<PLAYER, 4> member;

};

void process_packet(int c_uid, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACK* token_pack = reinterpret_cast<CS_LOGIN_PACK*>(packet);

		if (0 == strcmp(token_pack->Token, "theEnd")) {
			std::string nameBuffer = GetPlayerName(clients[c_uid].IdToken);

			if ("TokenError" == nameBuffer || "Token Error" == nameBuffer || "Empty" == nameBuffer) {
				SC_LOGIN_FAIL_PACK fail_pack;
				fail_pack.size = sizeof(SC_LOGIN_FAIL_PACK);
				fail_pack.type = SC_LOGIN_FAIL;
				clients[c_uid].do_send(&fail_pack);
				printf("Login Fail!\n");
			}
			else if (Login_UDB(c_uid, nameBuffer)) {
				{
					std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
					clients[c_uid]._state = ST_INGAME;
					clients[c_uid]._uid = c_uid;
				}

				SC_LOGIN_SUCCESS_PACK ok_pack;
				ok_pack.size = sizeof(ok_pack);
				ok_pack.type = SC_LOGIN_SUCCESS;
				clients[c_uid].do_send(&ok_pack);

				SC_LOGIN_INFO_PACK info_pack;
				info_pack.size = sizeof(SC_LOGIN_INFO_PACK);
				info_pack.type = SC_LOGIN_INFO;
				strncpy_s(info_pack.name, CHAR_SIZE, clients[c_uid]._name, CHAR_SIZE);
				strncpy_s(info_pack._q_skill, CHAR_SIZE, clients[c_uid]._q_skill, CHAR_SIZE);
				strncpy_s(info_pack._pet_num, CHAR_SIZE, clients[c_uid]._pet_num, CHAR_SIZE);
				info_pack._q_item = clients[c_uid]._q_item;
				info_pack._player_skin = clients[c_uid]._player_skin;
				clients[c_uid].do_send(&info_pack);
				
				printf("Player Name: %s Login!\n", clients[c_uid]._name);
			}
			else {
				if (SetNew_UDB(c_uid, nameBuffer)) {
					SetNew_ALL_ItemDB(c_uid, nameBuffer);

					{
						std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
						clients[c_uid]._state = ST_INGAME;
						clients[c_uid]._uid = c_uid;
						strncpy_s(clients[c_uid]._name, CHAR_SIZE, nameBuffer.c_str(), CHAR_SIZE);
					}

					SC_LOGIN_SUCCESS_PACK ok_pack;
					ok_pack.size = sizeof(ok_pack);
					ok_pack.type = SC_LOGIN_SUCCESS;
					clients[c_uid].do_send(&ok_pack);

					SC_LOGIN_INFO_PACK info_pack;
					info_pack.size = sizeof(SC_LOGIN_INFO_PACK);
					info_pack.type = SC_LOGIN_INFO;
					strncpy_s(info_pack.name, CHAR_SIZE, clients[c_uid]._name, CHAR_SIZE);
					strncpy_s(info_pack._q_skill, CHAR_SIZE, clients[c_uid]._q_skill, CHAR_SIZE);
					strncpy_s(info_pack._pet_num, CHAR_SIZE, clients[c_uid]._pet_num, CHAR_SIZE);
					info_pack._q_item = clients[c_uid]._q_item;
					info_pack._player_skin = clients[c_uid]._player_skin;
					clients[c_uid].do_send(&info_pack);
					printf("New player Name: %s Login!\n", clients[c_uid]._name);
				}
				else {
					SC_LOGIN_FAIL_PACK fail_pack;
					fail_pack.size = sizeof(SC_LOGIN_FAIL_PACK);
					fail_pack.type = SC_LOGIN_FAIL;
					clients[c_uid].do_send(&fail_pack);
					printf("Login Fail!\n");
				}
			}
		}
		else {
			std::string tokenBuffer;
			tokenBuffer.assign(token_pack->Token, (size_t)token_pack->Token_size);
			clients[c_uid].IdToken.append(tokenBuffer);
			int value = WSAGetLastError();
		}
 	}
	break;
	case CS_QUEST_INVENTORY:
	{
		Get_ALL_ItemDB(c_uid);
		 
		SC_ITEM_INFO_PACK item_pack;
		item_pack.size = sizeof(SC_ITEM_INFO_PACK);
		item_pack.type = SC_ITEM_INFO;

		for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
			for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {

				std::string item_name = Get_ItemName(category, item_num);
				int cnt = clients[c_uid].get_item_arrayName(category)[item_num];

				if (false == item_name.compare("None")) continue;
				if (0 == cnt) continue;

				printf("[%s] : %d\n", item_name.c_str(), cnt);
				{
					std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
					clients[c_uid].itemData.insert({ item_name, cnt });
				}

				strncpy_s(item_pack._name, CHAR_SIZE, item_name.c_str(), strlen(item_name.c_str()));
				item_pack._cnt = cnt;
				clients[c_uid].do_send(&item_pack);
			}
		}

		strncpy_s(item_pack._name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
		clients[c_uid].do_send(&item_pack);
		std::cout << "End\n";
	}
	break;
	case CS_SAVE_INVENTORY:
	{
		CS_SAVE_INVENTORY_PACK* save_pack = reinterpret_cast<CS_SAVE_INVENTORY_PACK*>(packet);
		short cnt = save_pack->_cnt;
		
		break;

		std::map<std::string, short>::iterator info = clients[c_uid].itemData.find(save_pack->_name);
		if (clients[c_uid].itemData.end() != info) {
			std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
			info->second = cnt;
		}
		else if (0 == strcmp(save_pack->_name, "theEnd")) {
			for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
				for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {

					std::map<std::string, short>::iterator it = clients[c_uid].itemData.find(Get_ItemName(category, item_num));
					if (clients[c_uid].itemData.end() != it) {
						clients[c_uid].get_item_arrayName(category)[item_num] = it->second;
						std::cout << "<" << it->first << "> - " << it->second << std::endl;
					}
					else {
						clients[c_uid].get_item_arrayName(category)[item_num] = 0;
						std::cout << "<" << it->first << "> - " << 0 << std::endl;
					}
				}
			}
		}
	}
	break;
	case CS_QUEST_STORAGE:
	{
		Get_ALL_StorageDB(c_uid);

		SC_ITEM_INFO_PACK item_pack;
		item_pack.size = sizeof(SC_ITEM_INFO_PACK);
		item_pack.type = SC_ITEM_INFO;

		for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
			for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {

				std::string item_name = Get_ItemName(category, item_num);
				int cnt = clients[c_uid].get_storage_item_arrayName(category)[item_num];
				if (false == item_name.compare("None"))continue;
				else if (0 == cnt)							continue;

				printf("[%s] : %d\n", item_name.c_str(), cnt);

				strncpy_s(item_pack._name, CHAR_SIZE, item_name.c_str(), strlen(item_name.c_str()));
				item_pack._cnt = cnt;
				clients[c_uid].do_send(&item_pack);
			}
		}

		strncpy_s(item_pack._name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
		clients[c_uid].do_send(&item_pack);
	}
	break;
	case CS_PARTY_SEARCHING:
	{
		SC_PARTIES_INFO_PACK party_list;
		party_list.size = sizeof(SC_PARTIES_INFO_PACK);
		party_list.type = SC_PARTY_LIST_INFO;

		for (int i = 0; i < 8; ++i) {
			std::string nameBuffer = std::string("Room_0").append(std::to_string(i + 1));

			strncpy_s(party_list._name, nameBuffer.c_str(), strlen(nameBuffer.c_str()));
			party_list._staff_count = static_cast<char>(parties[i]._mem_count);

			{
				std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
				if (parties[i]._inStage) party_list.Inaccessible = 1;
				else party_list.Inaccessible = 0;
			}

			clients[c_uid].do_send(&party_list);
		}

		strncpy_s(party_list._name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
		clients[c_uid].do_send(&party_list);
	}
	break;
	case CS_PARTY_ENTER:
	{
		CS_PARTY_ENTER_PACK* party_info = reinterpret_cast<CS_PARTY_ENTER_PACK*>(packet);
		int party_number = static_cast<int>(party_info->party_num);
		if (0 > party_number || party_number > 8) break;

		int cur_party_member_count = parties[party_number]._mem_count;
		if (0 <= cur_party_member_count && cur_party_member_count <= 3) {

			strncpy_s(parties[party_number].member[cur_party_member_count]._name, CHAR_SIZE, clients[c_uid]._name, CHAR_SIZE);
			parties[party_number].member[cur_party_member_count]._uid = c_uid;

			strncpy_s(parties[party_number].member[cur_party_member_count]._pet_num, CHAR_SIZE, clients[c_uid]._pet_num, CHAR_SIZE);
			{
				std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
				parties[party_number].member[cur_party_member_count]._player_state = ST_NOTREADY;
				clients[c_uid]._player_state = ST_NOTREADY;
			}
			
			parties[party_number]._mem_count += 1;

			SC_PARTY_ENTER_OK_PACK ok_pack;
			ok_pack.size = sizeof(SC_PARTY_ENTER_OK_PACK);
			ok_pack.type = SC_PARTY_ENTER_OK;
			clients[c_uid].do_send(&ok_pack);
			clients[c_uid]._party_num = party_number;

			printf("%s party connection Success\n", clients[c_uid].get_name());
		}
		else {
			printf("%s party connection failed\n", clients[c_uid].get_name());
		}
		
	}
	break;
	case CS_PARTY_INFO:
	{
		SC_PARTY_INFO_PACK in_party;
		in_party.size = sizeof(SC_PARTY_INFO_PACK);
		in_party.type = SC_PARTY_INFO;

		for (SESSION& cl : parties[clients[c_uid]._party_num].member) {
			if (0 == strcmp("None", cl._name)) continue;
			else  if (-1 == cl._uid)						continue;

			strncpy_s(in_party._mem, CHAR_SIZE, cl._name, CHAR_SIZE);
			strncpy_s(in_party._mem_pet, CHAR_SIZE, cl._pet_num, CHAR_SIZE);
			
			{
				std::lock_guard<std::mutex> ll{ cl._lock };
				if (ST_READY == cl._player_state) {
					in_party._mem_state = 2;
				}
				else in_party._mem_state = 0;
			}

			clients[c_uid].do_send(&in_party);
		}

		strncpy_s(in_party._mem, CHAR_SIZE, "theEnd", strlen("theEnd"));
		strncpy_s(in_party._my_name, CHAR_SIZE, clients[c_uid]._name, strlen(clients[c_uid]._name));
		clients[c_uid].do_send(&in_party);
	}
	break;
	case CS_PARTY_READY:
	{
		int cur_member = 0;
		int ready_member = 0;
		int party_num = clients[c_uid]._party_num;
		
		{
			std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
			if (clients[c_uid]._player_state != ST_READY) {
				clients[c_uid]._player_state = ST_READY;
			}
		}
		
		SC_PARTY_JOIN_RESULT_PACK result_pack;
		result_pack.size = sizeof(SC_PARTY_JOIN_RESULT_PACK);
		result_pack.type = SC_PARTY_JOIN_SUCCESS;

		int curMem = 0;
		for (SESSION& cl : parties[party_num].member) {
			if (0 != strcmp("None", cl._name)) {
				cur_member += 1;
			}

			if (c_uid == cl._uid) {
				cl._player_state = ST_READY;
				result_pack.memberState[curMem] = 2;
			}

			{
				if (ST_READY == cl._player_state) {
					ready_member += 1;
					result_pack.memberState[curMem] = 2;
				}
				else result_pack.memberState[curMem] = 0;
			}

			curMem += 1;
		}

		// if ((ready_member == curMem) && (false == parties[party_num]._inStage)) {
		if ((ready_member == cur_member)) {
			result_pack._result = 1;
			parties[party_num]._inStage = true;
			for (SESSION& cl : parties[party_num].member) {
				if (0 != strcmp("None", cl._name)) {
					printf("%s in Stage!\n", clients[cl._uid]._name);
					clients[cl._uid].do_send(&result_pack);
				}
			}
		}
		// else if(false == parties[party_num]._inStage) {
		else {
			result_pack._result = 0;
			
			for (SESSION& cl : parties[party_num].member) {
				if (0 != strcmp("None", cl._name)) {
					clients[cl._uid].do_send(&result_pack);
				}
			}
		}
	}
	break;
	case CS_PARTY_LEAVE:
	{
		int party_num = clients[c_uid]._party_num;

		if (0 <= party_num) {
			if (parties[party_num].leave_member(clients[c_uid]._name)) {
				clients[c_uid]._player_state = ST_HOME;
				clients[c_uid]._party_num = -1;

				SC_PARTY_LEAVE_SUCCESS_PACK leave_pack;
				leave_pack.size = sizeof(SC_PARTY_LEAVE_SUCCESS_PACK);
				leave_pack.type = SC_PARTY_LEAVE_SUCCESS;
				strncpy_s(leave_pack._mem, CHAR_SIZE, clients[c_uid]._name, CHAR_SIZE);
				clients[c_uid].do_send(&leave_pack);
				printf("[%s] left the party.\n", clients[c_uid].get_name());
			}
			else {
				SC_PARTY_LEAVE_FAIL_PACK fail_leave_pack;
				fail_leave_pack.size = sizeof(SC_PARTY_LEAVE_FAIL_PACK);
				fail_leave_pack.type = SC_PARTY_LEAVE_FAIL;
				clients[c_uid].do_send(&fail_leave_pack);
			}
		}
		else {
			printf("[%s] not [%d]party member.\n", clients[c_uid].get_name(), party_num);
			SC_PARTY_LEAVE_FAIL_PACK fail_leave_pack;
			fail_leave_pack.size = sizeof(SC_PARTY_LEAVE_FAIL_PACK);
			fail_leave_pack.type = SC_PARTY_LEAVE_FAIL;
			clients[c_uid].do_send(&fail_leave_pack);
		}
	}
	break;
	case CS_LOGOUT:
	{
		CS_LOGOUT_PACK* logout_client = reinterpret_cast<CS_LOGOUT_PACK*>(packet);

		SC_LOGOUT_RESULT_PACK out_client;
		out_client.size = sizeof(SC_LOGOUT_RESULT_PACK);
		out_client.type = SC_LOGOUT_RESULT;
		if (Set_ALL_ItemDB(c_uid) && Logout_UDB(c_uid)) {
			printf("[Success]->SaveData Logout %s\n", clients[c_uid].get_name());
		}
		else {
			printf("[Fail]->SaveData Logout %s\n", clients[c_uid].get_name());
		}
		{
			std::lock_guard<std::mutex> ll{ clients[c_uid]._lock };
			strcpy_s(out_client._result, "1");
			clients[c_uid].do_send(&out_client);
		}
		disconnect(c_uid);
	}
	break;
	}
}

void disconnect(int c_uid)
{
	// client 정보 정리
	closesocket(clients[c_uid]._socket);
	clients[c_uid].clear();
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
				std::cout << "\nGQCS Error on client[" << key << "]\n\n";
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
		case RECV:
		{
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
		case SEND:		
			delete ex_over;
			break;
		}
	}
}
