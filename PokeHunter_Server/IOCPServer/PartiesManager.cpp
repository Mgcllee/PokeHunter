#include "PartiesManager.h"

PartiesManager::~PartiesManager() {

}

void PartiesManager::get_party_list(int player_ticket)
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

	player_manager->get_player(player_ticket)->get_session()->send_packet(&party_list);
}

void PartiesManager::enter_party(int user_id, char* packet) {
	int party_number = reinterpret_cast<CS_PARTY_ENTER_PACK*>(packet)->party_num;
	
	if (party_number < 0 || party_number > 8) {
		return;
	}

	parties[party_number].enter_member(user_id, packet);
}
