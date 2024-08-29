#include "Player.h"

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
	//TODO: make disconnect and recycle Player
}

void Player::check_exists_token(char* login_token)
{
	if (0 == strncmp(login_token, "theEnd", strlen("theEnd")))
	{
		// TODO: get player name
		// std::string nameBuffer = GetPlayerName(*get_cognito_id_token());
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

			// TODO: get item name and insert player item, and send item information packet
			// std::string itemname = Get_ItemName(category, item_num);
			
			clients[user_id].get_session()->send_packet(&item_pack);
		}
	}

	strncpy_s(item_pack.name, CHAR_SIZE, "theEnd", sizeof("theEnd"));
	clients[user_id].get_session()->send_packet(&item_pack);
}

bool Player::set_inventory_item(char* item_name, char item_count)
{
	// set each item
	return false;
}

void Player::get_all_storage_item()
{
	SC_ITEM_INFO_PACK item_pack;
	item_pack.size = sizeof(SC_ITEM_INFO_PACK);
	item_pack.type = SC_ITEM_INFO;

	for (int category = 0; category < MAX_ITEM_CATEGORY; ++category) {
		for (int item_num = 0; item_num < MAX_ITEM_COUNT; ++item_num) {

			// TODO: send each item informtaion
			// std::string itemname = Get_ItemName(category, item_num);

			clients[user_id].do_send(&item_pack);
		}
	}
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

bool Player::operator== (const Player& rhs) const
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