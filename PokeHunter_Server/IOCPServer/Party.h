#pragma once

#include "netModule.h"
#include "Player.h"

class Party
{
public:
	Party();
	~Party();

	bool enter_member(Player& new_mem);
	bool leave_member(char* mem_name);
	bool get_party_in_stage();
	short get_member_count();

	std::list<Player> member;

private:
	char name[CHAR_SIZE];
	short member_count;
	bool in_stage;

	std::mutex ll;
};

