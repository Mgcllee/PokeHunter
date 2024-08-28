#pragma once

constexpr int MAX_USER = 100;
constexpr int MAX_PARTY = 100;
constexpr int BUF_SIZE = 128;
constexpr int PORT_NUM = 7777;

constexpr short MAX_ITEM_CATEGORY = 4;
constexpr short MAX_ITEM_COUNT = 9;

constexpr short CHAR_SIZE = 20;

constexpr char CS_LOGIN = 0;

constexpr char CS_PARTY_SEARCHING = 2;
constexpr char CS_PARTY_INFO = 3;

constexpr char CS_PARTY_ENTER = 4;
constexpr char SC_PARTY_ENTER_OK = 5;

constexpr char CS_PARTY_READY = 6;
constexpr char CS_PARTY_LEAVE = 8;

constexpr char CS_QUEST_INVENTORY = 9;
constexpr char CS_SAVE_INVENTORY = 10;
constexpr char CS_LOGOUT = 11;
constexpr char CS_QUEST_STORAGE = 12;

/////////////////////////////////////////////

constexpr char SC_LOGIN_FAIL = 20;
constexpr char SC_LOGIN_SUCCESS = 21;
constexpr char SC_LOGIN_INFO = 22;
constexpr char SC_PARTY_LIST_INFO = 23;
constexpr char SC_PARTY_INFO = 24;
constexpr char SC_PARTY_STAFF_READY = 25;
constexpr char SC_PARTY_START = 26;
constexpr char SC_PARTY_JOIN_FAIL = 27;
constexpr char SC_PARTY_JOIN_SUCCESS = 28;
constexpr char SC_PARTY_LEAVE_FAIL = 29;
constexpr char SC_PARTY_LEAVE_SUCCESS = 30;
constexpr char SC_LOGOUT_RESULT = 31;
constexpr char SC_ITEM_INFO = 33;


#pragma pack (push, 1)

struct CS_LOGIN_PACK {
	char size;
	char type;

	char Token[60];
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

	char readyState;
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

struct SC_LOGIN_FAIL_PACK {
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
	char _pet_num[CHAR_SIZE];

	char _q_item;
	char _q_skill[CHAR_SIZE];
};

struct SC_PARTIES_INFO_PACK {	// 파티의 요약정보
	char size;
	char type;

	char _name[CHAR_SIZE];		// 파티의 이름
	char _staff_count;			// 파티원 수
	char Inaccessible;			// 파티 출발 여부
};

struct SC_PARTY_INFO_PACK {		// 각 파티원의 세부 정보
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

struct SC_PARTY_LEAVE_FAIL_PACK {
	char size;
	char type;
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

struct CS_LOGOUT_RESULT_PACK {
	char size;
	char type;
};

struct SC_ITEM_INFO_PACK {
	char size;
	char type;

	char _name[CHAR_SIZE];
	char _cnt;
};

#pragma pack (pop)
