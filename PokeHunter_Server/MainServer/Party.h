#pragma once

#include "PlayerManager.h"
#include "netModule.h"

class Party {
 public:
  Party();
  Party(PlayerManager* players) : player_manager(players) {}
  ~Party();

  bool enter_member(int user_ticket, char* packet);
  bool leave_member(char* mem_name);
  bool get_party_in_stage();
  short get_member_count();

 private:
  char name[CHAR_SIZE];
  short member_count;
  bool in_stage;

  std::list<Player*> member;

  PlayerManager* player_manager;

  std::mutex ll;
};