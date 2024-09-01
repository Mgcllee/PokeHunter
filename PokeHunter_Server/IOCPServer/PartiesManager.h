#pragma once

#include "Party.h"
#include "PlayerManager.h"

class PartiesManager {
 public:
  PartiesManager(PlayerManager* players) : player_manager(players) {}
  ~PartiesManager();

  void get_party_list(int user_id);
  void enter_party(int user_id, char* packet);

 private:
  PlayerManager* player_manager;
  std::array<Party, MAX_PARTY> parties;
};
