#include "Party.h"

Party::Party()
	: member_count(0)
	, in_stage(false)
	, name("Empty")
{

}

Party::~Party()
{

}

bool Party::enter_member(Player& new_mem)
{
	if (member_count >= PARTY_MAX_NUM)
		return false;

	if (0 <= member_count && member_count <= 3) {

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
