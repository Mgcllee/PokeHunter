#pragma once

#include "netModule.h"

class Player {
public:
	Player();
	Player(const Player& rhs);
	~Player();

	Player& operator=(const Player& ref);
	bool operator== (const Player& rhs) const;

	void lock() { ll.lock(); }
	void unlock() { ll.unlock(); }

	void set_user_id(int new_user_id) { user_id = new_user_id; }
	Session* get_session();
	std::string* get_cognito_id_token();

	char* get_user_name();
	void set_user_name(const char* in);

	void set_item(const char* in_item_name, short index, char cnt);
	short get_item();
	short get_storage_item();
	void send_fail_reason(const char* fail_reason = "NONE");
	void send_self_user_info(const char* success_message = "NONE");

	void recycle_player();

	void check_exists_token(char*);

	void get_all_inventory_item();
	bool set_inventory_item(char* item_name, char item_count);

	void get_all_storage_item();

	void set_ready_in_party();
	void leave_current_party();

private:

public:

private:
	mutable std::mutex ll;

	char name[CHAR_SIZE];
	short user_id;
	char pet_type[CHAR_SIZE];
	char player_skin_type;

	std::unordered_map<std::string, short> inventory_data;
	std::unordered_map<std::string, short> storage_data;

	char party_num;

	std::string cognito_id_token;

	PLAYER_STATE player_state;

	Session session;
};


