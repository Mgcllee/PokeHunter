#pragma once

#include "Player.h"

class PlayerManager {
 public:
  PlayerManager();
  ~PlayerManager();

  void init_player(SOCKET socet, int player_ticket);
  Player* get_player(int user_ticket);

  void send_all_player(void* packet);

 private:
  std::array<Player, MAX_USER> clients;
};
