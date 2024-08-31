#include "PlayerManager.h"

PlayerManager::PlayerManager()
{
}

PlayerManager::~PlayerManager()
{
}

void PlayerManager::init_player(SOCKET socet, int player_ticket)
{
	clients[player_ticket].get_session()->set_socket(socet);
	clients[player_ticket].set_user_id(player_ticket);

}

Player* PlayerManager::get_player(int user_ticket)
{
	return &clients[user_ticket];
}

void PlayerManager::send_all_player(void* packet)
{
	for (Player& p : clients)
	{
		p.get_session()->send_packet(&packet);
	}
}
