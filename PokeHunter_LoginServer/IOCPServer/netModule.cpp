#pragma once

#include "DBModule.h"
#include "AWSModule.h"

std::array<Player, MAX_USER> clients;
std::array<Party, MAX_PARTY> parties;

OverlappedExpansion::OverlappedExpansion()
	: socket_type(SOCKET_TYPE::RECV)
	, wsa_buffer(WSABUF(BUF_SIZE, packet_buffer))
{
	ZeroMemory(&overlapped, sizeof(overlapped));
}

OverlappedExpansion::OverlappedExpansion(char* packet) 
	: socket_type(SOCKET_TYPE::SEND)
	, wsa_buffer(WSABUF(packet[0], packet_buffer))
{
	ZeroMemory(&overlapped, sizeof(overlapped));
	memcpy(packet_buffer, packet, packet[0]);
}

OverlappedExpansion::~OverlappedExpansion()
{

}

Session::Session()
	: remain_packet_size(0)
	, socket(NULL)
{
	
}
Session::~Session()
{
	closesocket(socket);
}

bool Session::recycle_session()
{
	return false;
}

Session& Session::operator=(const Session& ref)
{
	this->overlapped = ref.overlapped;
	this->socket_type = ref.socket_type;
	this->wsa_buffer = ref.wsa_buffer;
	
	strncpy(this->packet_buffer, ref.packet_buffer, BUF_SIZE);

	this->socket = ref.socket;
	this->remain_packet_size = ref.remain_packet_size;

	return *this;
}

void Session::set_socket(SOCKET new_socket)
{
	socket = new_socket;

}

int Session::get_prev_remain_packet_size()
{
	return remain_packet_size;
}

void Session::set_curr_remain_packet_size(int curr_remain_packet_size)
{
	remain_packet_size = curr_remain_packet_size;
}

void Session::recv_packet()
{
	DWORD recv_flag = 0;
	memset(&overlapped, 0, sizeof(overlapped));
	wsa_buffer.len = BUF_SIZE - remain_packet_size;
	wsa_buffer.buf = packet_buffer + remain_packet_size;
	socket_type = RECV;
	WSARecv(socket, &wsa_buffer, 1, 0, &recv_flag, &overlapped, 0);
}

void Session::send_packet(void* packet)
{
	if (packet == nullptr || socket == NULL) return;

	OverlappedExpansion* sendoverlapped = new OverlappedExpansion{ reinterpret_cast<char*>(packet) };
	WSASend(socket, &sendoverlapped->wsa_buffer, 1, 0, 0, &sendoverlapped->overlapped, 0);
}

void Session::send_all_client(void* packet)
{
	for (Player& p : clients)
	{
		p.get_session()->send_packet(&packet);
	}
}

void Session::disconnect()
{
	closesocket(socket);
}

Player::Player()
	: user_id(-1)
	, party_num(-1)
	, name("None")
	, pet_type("None")
	, player_state(HOME)
{
	
}
Player::Player(const Player& rhs)
	: user_id(-1)
	, party_num(-1)
	, name("None")
	, pet_type("None")
	, player_state(HOME)
{

}
Player::~Player()
{

}
void Player::recycle_player()
{
	disconnect(user_id);
}

void Player::check_exists_token(char* login_token)
{
	if (0 == strncmp(login_token, "theEnd", strlen("theEnd")))
	{
		std::string nameBuffer = GetPlayerName(*get_cognito_id_token());
		
		if ("TokenError" == nameBuffer || "Token Error" == nameBuffer || "Empty" == nameBuffer) {
			clients[user_id].send_fail_reason("Token error");
		}
		else if (Login_UDB(user_id, nameBuffer)) {
			clients[user_id].send_self_user_info(
				std::string("Login player name is ").append(clients[user_id].getname()).c_str()
			);
		}
		else {
			if (SetNew_UDB(user_id, nameBuffer)) {
				clients[user_id].send_self_user_info(
					std::string("New login player name is ").append(clients[user_id].getname()).c_str()
				);
			}
			else {
				clients[user_id].send_fail_reason("Failed to initialize player");
			}
		}
	}
	else
	{
		std::string tokenBuffer;
		tokenBuffer.assign(login_token, (size_t)login_token);
		clients[user_id].get_cognito_id_token()->append(tokenBuffer);
	}
}

void Player::get_all_inventory_item()
{
	SC_ITEM_INFO_PACK item_pack;
	item_pack.size = sizeof(SC_ITEM_INFO_PACK);
	item_pack.type = SC_ITEM_INFO;
	
	for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
		for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {

			std::string itemname = Get_ItemName(category, item_num);
			int cnt = clients[user_id].get_item_arrayName(category)[item_num];

			if (false == itemname.compare("None")) continue;
			if (0 == cnt) continue;

			{
				clients[user_id].lock();
				clients[user_id].get_item().insert({itemname, cnt});
				clients[user_id].unlock();
			}

			strncpy_s(item_pack.name, CHAR_SIZE, itemname.c_str(), strlen(itemname.c_str()));
			item_pack._cnt = cnt;
			clients[user_id].get_session()->send_packet(&item_pack);
		}
	}

	strncpy_s(item_pack.name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
	clients[user_id].get_session()->send_packet(&item_pack);
}

bool Player::set_inventory_item(char* item_name, char item_count)
{
	std::map<std::string, short>::iterator info = clients[user_id].itemData.find(save_pack->name);
	if (clients[user_id].itemData.end() != info) {
		std::lock_guard<std::mutex> ll{ clients[user_id]._lock };
		info->second = cnt;
	}
	else if (0 == strcmp(save_pack->name, "theEnd")) {
		for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
			for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {

				std::map<std::string, short>::iterator it = clients[user_id].itemData.find(Get_ItemName(category, item_num));
				if (clients[user_id].itemData.end() != it) {
					clients[user_id].get_item_arrayName(category)[item_num] = it->second;
					std::cout << "<" << it->first << "> - " << it->second << std::endl;
				}
				else {
					clients[user_id].get_item_arrayName(category)[item_num] = 0;
					std::cout << "<" << it->first << "> - " << 0 << std::endl;
				}
			}
		}
	}
	return false;
}

void Player::get_all_storage_item()
{
	// Get_ALL_StorageDB(user_id);

	SC_ITEM_INFO_PACK item_pack;
	item_pack.size = sizeof(SC_ITEM_INFO_PACK);
	item_pack.type = SC_ITEM_INFO;

	/*

	for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
		for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {

			std::string itemname = Get_ItemName(category, item_num);
			int cnt = clients[user_id].get_storage_item_arrayName(category)[item_num];
			if (false == itemname.compare("None"))continue;
			else if (0 == cnt)							continue;

			printf("[%s] : %d\n", itemname.c_str(), cnt);

			strncpy_s(item_pack.name, CHAR_SIZE, itemname.c_str(), strlen(itemname.c_str()));
			item_pack._cnt = cnt;
			clients[user_id].do_send(&item_pack);
		}
	}

	strncpy_s(item_pack.name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
	clients[user_id].do_send(&item_pack);

	*/
}

void Player::enter_party(int party_number)
{
	if (0 > party_number || party_number > 8) return;

	int cur_party_member_count = parties[party_number].get_member_count();
	if (0 <= cur_party_member_count && cur_party_member_count <= 3) {

		/*strncpy_s(parties[party_number].member[cur_party_member_count].name, CHAR_SIZE, clients[user_id].name, CHAR_SIZE);
		parties[party_number].member[cur_party_member_count].user_id = user_id;

		strncpy_s(parties[party_number].member[cur_party_member_count]._pet_num, CHAR_SIZE, clients[user_id]._pet_num, CHAR_SIZE);
		{
			std::lock_guard<std::mutex> ll{ clients[user_id]._lock };
			parties[party_number].member[cur_party_member_count]._player_state = ST_NOTREADY;
			clients[user_id]._player_state = ST_NOTREADY;
		}

		parties[party_number].mem_count += 1;

		SC_PARTY_ENTER_OK_PACK ok_pack;
		ok_pack.size = sizeof(SC_PARTY_ENTER_OK_PACK);
		ok_pack.type = SC_PARTY_ENTER_OK;
		clients[user_id].do_send(&ok_pack);
		clients[user_id]._party_num = party_number;

		printf("%s party connection Success\n", clients[user_id].getname());*/


		// CS_PARTY_INFO
		SC_PARTY_INFO_PACK in_party;
		in_party.size = sizeof(SC_PARTY_INFO_PACK);
		in_party.type = SC_PARTY_INFO;

		/*

		for (Session& cl : parties[clients[user_id]._party_num].member) {
			if (0 == strcmp("None", cl.name)) continue;
			else  if (-1 == cl.user_id)						continue;

			strncpy_s(in_party._mem, CHAR_SIZE, cl.name, CHAR_SIZE);
			strncpy_s(in_party._mem_pet, CHAR_SIZE, cl._pet_num, CHAR_SIZE);

			{
				std::lock_guard<std::mutex> ll{ cl._lock };
				if (ST_READY == cl._player_state) {
					in_party._mem_state = 2;
				}
				else in_party._mem_state = 0;
			}

			clients[user_id].do_send(&in_party);
		}

		strncpy_s(in_party._mem, CHAR_SIZE, "theEnd", strlen("theEnd"));
		strncpy_s(in_party._myname, CHAR_SIZE, clients[user_id].name, strlen(clients[user_id].name));
		clients[user_id].do_send(&in_party);

		*/
	}
	else {
		// printf("%s party connection failed\n", clients[user_id].getname());
	}
}

void Player::set_ready_in_party()
{
	/*

		int cur_member = 0;
		int ready_member = 0;
		int party_num = clients[user_id]._party_num;

		{
			std::lock_guard<std::mutex> ll{ clients[user_id]._lock };
			if (clients[user_id]._player_state != ST_READY) {
				clients[user_id]._player_state = ST_READY;
			}
		}

		SC_PARTY_JOIN_RESULT_PACK result_pack;
		result_pack.size = sizeof(SC_PARTY_JOIN_RESULT_PACK);
		result_pack.type = SC_PARTY_JOIN_SUCCESS;

		int curMem = 0;
		for (Session& cl : parties[party_num].member) {
			if (0 != strcmp("None", cl.name)) {
				cur_member += 1;
			}

			if (user_id == cl.user_id) {
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
			for (Session& cl : parties[party_num].member) {
				if (0 != strcmp("None", cl.name)) {
					printf("%s in Stage!\n", clients[cl.user_id].name);
					clients[cl.user_id].do_send(&result_pack);
				}
			}
		}
		// else if(false == parties[party_num]._inStage) {
		else {
			result_pack._result = 0;

			for (Session& cl : parties[party_num].member) {
				if (0 != strcmp("None", cl.name)) {
					clients[cl.user_id].do_send(&result_pack);
				}
			}
		}

		*/
}

void Player::leave_current_party()
{
	/*

		int party_num = clients[user_id]._party_num;

		if (0 <= party_num) {
			if (parties[party_num].leave_member(clients[user_id].name)) {
				clients[user_id]._player_state = ST_HOME;
				clients[user_id]._party_num = -1;

				SC_PARTY_LEAVE_SUCCESS_PACK leave_pack;
				leave_pack.size = sizeof(SC_PARTY_LEAVE_SUCCESS_PACK);
				leave_pack.type = SC_PARTY_LEAVE_SUCCESS;
				strncpy_s(leave_pack._mem, CHAR_SIZE, clients[user_id].name, CHAR_SIZE);
				clients[user_id].do_send(&leave_pack);
				printf("[%s] left the party.\n", clients[user_id].getname());
			}
			else {
				SC_PARTY_LEAVE_FAIL_PACK fail_leave_pack;
				fail_leave_pack.size = sizeof(SC_PARTY_LEAVE_FAIL_PACK);
				fail_leave_pack.type = SC_PARTY_LEAVE_FAIL;
				clients[user_id].do_send(&fail_leave_pack);
			}
		}
		else {
			printf("[%s] not [%d]party member.\n", clients[user_id].getname(), party_num);
			SC_PARTY_LEAVE_FAIL_PACK fail_leave_pack;
			fail_leave_pack.size = sizeof(SC_PARTY_LEAVE_FAIL_PACK);
			fail_leave_pack.type = SC_PARTY_LEAVE_FAIL;
			clients[user_id].do_send(&fail_leave_pack);
		}

		*/

}

Session* Player::get_session()
{
	return &session;
}
std::string* Player::get_cognito_id_token()
{
	return &cognito_id_token;
}
Player& Player::operator=(const Player& ref)
{
	if (this != &ref)
	{
		std::lock(this->ll, ref.ll);
		std::lock_guard<std::mutex> this_lock(this->ll, std::adopt_lock);
		std::lock_guard<std::mutex> ref_lock(ref.ll, std::adopt_lock);

		strncpy_s(this->name, ref.name, strlen(ref.name));
		this->user_id = ref.user_id;
		strncpy_s(this->pet_type, ref.pet_type, CHAR_SIZE);
		
		this->player_skin_type = ref.player_skin_type;
		this->player_state = ref.player_state;
		this->session = ref.session;
		this->inventory_data = ref.inventory_data;
		this->storage_data = ref.storage_data;
	}

	return *this;
}

bool Player::operator== (const Player & rhs) const
{
	return user_id == rhs.user_id;
}

char* Player::get_user_name()
{
	return name;
}

void Player::set_user_name(const char* in)
{
	strncpy_s(name, sizeof(in), in, sizeof(in));
}

void Player::set_item(const char* in_itemname, short index, char cnt)
{
	
}

short Player::get_item()
{
	return 0;
}

short Player::get_storage_item()
{
	return 0;
}

void Player::send_fail_reason(const char* fail_reason)
{
	SC_FAIL_PACK fail_pack;
	fail_pack.size = sizeof(SC_FAIL_PACK);
	fail_pack.type = SC_FAIL;
	session.send_packet(&fail_pack);
	printf("[Fail Log][%d] : %s\n", user_id, fail_reason);
}

void Player::send_self_user_info(const char* success_message)
{
	SC_LOGIN_INFO_PACK info_pack = SC_LOGIN_INFO_PACK();
	info_pack.size = (char)sizeof(info_pack);
	info_pack.type = SC_LOGIN_INFO;

	strcpy_s(info_pack.name, name);
	info_pack._player_skin = 1;
	info_pack._pet_num = 1;
	
	session.send_packet(&info_pack);

	printf("[Success Log][%d] : %s\n", user_id, success_message);
}

Party::Party()
	: member_count(0)
	, in_stage(false)
	, name("Empty")
{

}

Party::~Party()
{
	
}

bool Party::new_member(Player& new_mem)
{
	if (member_count >= PARTY_MAX_NUM)
		return false;
	
	ll.lock();
	member.push_back(new_mem);
	++member_count;
	ll.unlock();

	return true;
}

bool Party::leave_member(char* memname) {
	if (member_count <= 0)
		return false;

	ll.lock();
	for (Player& mem : member) 
	{
		if (0 == strcmp(mem.get_user_name(), memname)) 
		{
			member.remove(mem);
			if (0 == --member_count)
				in_stage = false;

			ll.unlock();
			return true;
		}
	}

	ll.unlock();
	return false;
}
bool Party::get_party_in_stage()
{
	return in_stage;
}
short Party::get_member_count()
{
	return member_count;
}

void PacketWorker::process_packet(int user_id, char* packet)
{
	switch (packet[1]) 
	{
	case CS_LOGIN:
		CS_LOGIN_PACK* token_packet = reinterpret_cast<CS_LOGIN_PACK*>(packet);
		clients[user_id].check_exists_token(token_packet->Token);
		break;

	case CS_CHAT:
		CS_CHAT_PACK* chatting_packet = reinterpret_cast<CS_CHAT_PACK*>(packet);
		sync_new_chatting_all_client(user_id, chatting_packet->content);
		break;

	case CS_QUEST_INVENTORY:
		clients[user_id].get_all_inventory_item();
		break;

	case CS_SAVE_INVENTORY:
		CS_SAVE_INVENTORY_PACK* item_packet = reinterpret_cast<CS_SAVE_INVENTORY_PACK*>(packet);
		clients[user_id].set_inventory_item(item_packet->_name, item_packet->_cnt);
		break;

	case CS_QUEST_STORAGE:
		clients[user_id].get_all_storage_item();
		break;

	case CS_PARTY_SEARCHING:
		get_party_list(user_id);	
		break;

	case CS_PARTY_ENTER:
		CS_PARTY_ENTER_PACK* party_info = reinterpret_cast<CS_PARTY_ENTER_PACK*>(packet);
		clients[user_id].enter_party(party_info->party_num);
		break;

	case CS_PARTY_READY:
		clients[user_id].set_ready_in_party();
		break;

	case CS_PARTY_LEAVE:
		clients[user_id].leave_current_party();
		break;

	case CS_LOGOUT:
		clients[user_id].recycle_player();
		break;
	}
}

void PacketWorker::sync_new_chatting_all_client(int user_id, std::string content)
{
	std::string chat_log = std::string("[채팅][").append(clients[user_id].get_user_name()).append("]: ").append(content);

	SC_CHAT_PACK ctp;
	ctp.size = sizeof(SC_CHAT_PACK);
	ctp.type = SC_CHAT;
	strcpy_s(ctp.content, chat_log.c_str());

	clients[user_id].get_session()->send_all_client(&ctp);
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

	ZeroMemory(&glbal_overlapped.overlapped, sizeof(glbal_overlapped.overlapped));
	int addr_size = sizeof(SOCKADDR_IN);
	AcceptEx(
		global_server_socket, global_client_accept_socket,
		glbal_overlapped.packet_buffer, 0, addr_size + 16, addr_size + 16, 0,
		&glbal_overlapped.overlapped
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

		clients[new_user_id].get_session()->set_socket(global_server_socket);
		clients[new_user_id].set_user_id(new_user_id);
		clients[new_user_id].unlock();

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(global_client_accept_socket), h_iocp, new_user_id, 0);

		clients[new_user_id].get_session()->recv_packet();
		global_client_accept_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}
	else
	{
		return;
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

void PacketWorker::worker_thread(HANDLE h_iocp)
{
	while (true) {
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OverlappedExpansion* exoverlapped = reinterpret_cast<OverlappedExpansion*>(over);

		if (FALSE == ret) {
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
		if ((0 == num_bytes) && (exoverlapped->socket_type == RECV)) continue;

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
			// TODO: make error message
			break;
		}
	}
}