#pragma once

enum SERVER_SOCKET_SETTINGS
{
	MAX_USER = 10,
	MAX_PARTY = 100,
	BUF_SIZE = 128,
	PORT_NUM = 7777,
	CHAR_SIZE = 64
};

constexpr short MAX_ITEM_CATEGORY = 4;
constexpr short MAX_ITEM_COUNT = 9;

enum SEND_CLIENT_TO_SERVER_PACKET_TYPE
{
	CS_LOGIN = 1,
	CS_PARTY_SEARCHING,
	CS_PARTY_INFO,
	CS_PARTY_ENTER,
	CS_PARTY_READY,
	CS_PARTY_LEAVE,
	CS_QUEST_INVENTORY,
	CS_SAVE_INVENTORY,
	CS_LOGOUT,
	CS_QUEST_STORAGE,
	CS_CHAT
};

enum SEND_SERVER_TO_CLIENT_PACKET_TYPE
{
	SC_PARTY_ENTER_OK = 1,
	SC_FAIL,
	SC_LOGIN_SUCCESS,
	SC_LOGIN_INFO,		
	SC_PARTY_LIST_INFO,
	SC_PARTY_INFO,	
	SC_PARTY_STAFF_READY,
	SC_PARTY_START,	
	SC_PARTY_JOIN_FAIL,
	SC_PARTY_JOIN_SUCCESS,
	SC_PARTY_LEAVE_FAIL,
	SC_PARTY_LEAVE_SUCCESS,
	SC_LOGOUT_RESULT,	
	SC_ITEM_INFO,			
	SC_CHAT
};

#define PARTY_MAX_NUM 4

#pragma pack (push, 1)

struct CS_LOGIN_PACK {
	char size;
	char type;

	char Token[CHAR_SIZE];
	char Token_size;
};

struct CS_QUEST_INVENTORY_PACK {
	char size;
	char type;
};

struct CS_SAVE_INVENTORY_PACK {
	char size;
	char type;

	char _name[CHAR_SIZE];
	char _cnt;
};

struct CS_PARTY_SEARCHING_PACK {
	char size;
	char type;
};

struct CS_PARTY_INFO_PACK {
	char size;
	char type;

	char party_num;
};

struct CS_PARTY_ENTER_PACK {
	char size;
	char type;

	char party_num;
};

struct SC_PARTY_ENTER_OK_PACK {
	char size;
	char type;
};

///////// 제거 대상 ////////
struct CS_PARTY_READY_PACK {
	char size;
	char type;

	char readyState = 0;
};

struct CS_CHAT_PACK {
	char size;
	char type;

	char content[CHAR_SIZE];
};

struct SC_CHAT_PACK {
	char size;
	char type;

	char content[CHAR_SIZE];
};

////////////////////////////

struct CS_PARTY_LEAVE_PACK {
	char size;
	char type;

	char party_num;
	char name[CHAR_SIZE];
};

struct CS_LOGOUT_PACK {
	char size;
	char type;

	char name[CHAR_SIZE];
};

struct CS_QUEST_STORAGE_PACK {
	char size;
	char type;
};

/////////////////////////////////////////

struct SC_FAIL_PACK {
	char size;
	char type;
};

struct SC_LOGIN_SUCCESS_PACK {
	char size;
	char type;
};

struct SC_LOGIN_INFO_PACK {
	char size;
	char type;
	char name[CHAR_SIZE];

	char _player_skin;
	char _pet_num;
};

struct SC_PARTIES_INFO_PACK {
	char size;
	char type;

	char _staff_count[MAX_PARTY];
	char Inaccessible[MAX_PARTY];
};

struct SC_PARTY_INFO_PACK {	
	char size;
	char type;

	char _my_name[CHAR_SIZE];
	char _mem[CHAR_SIZE];
	char _mem_pet[CHAR_SIZE];
	char _mem_state;
};

struct SC_PARTY_JOIN_RESULT_PACK {
	char size;
	char type;

	char _result;
	char memberState[4];
};

struct SC_PARTY_STAFF_READY_PACK {
	char size;
	char type;

	char _staff_num;
};

struct SC_PARTY_LEAVE_SUCCESS_PACK {
	char size;
	char type;

	char _mem[CHAR_SIZE];
	char _mem_pet;
	char _mem_state;
};

struct SC_LOGOUT_RESULT_PACK {
	char size;
	char type;

	char _result[CHAR_SIZE];
};

struct SC_ITEM_INFO_PACK {
	char size;
	char type;

	char name[CHAR_SIZE];
	char _cnt;
};

#pragma pack (pop)
