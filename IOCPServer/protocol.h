#pragma once

constexpr int MAX_USER					= 100;
constexpr int MAX_PARTY					= 100;
constexpr int BUF_SIZE					= 128;
constexpr int PORT_NUM					= 9999;

constexpr short MAX_ITEM_CATEGORY		= 4;
constexpr short MAX_ITEM_COUNT			= 9;

constexpr short CHAR_SIZE				= 20;
constexpr short CHAR_MIN_SIZE			= 10;

constexpr char CS_LOGIN					= 0;
constexpr char CS_MOVE					= 1;

constexpr char CS_PARTY_SEARCHING		= 2;
constexpr char CS_PARTY_INFO			= 3;
constexpr char CS_PARTY_READY			= 4;
constexpr char CS_PARTY_JOIN			= 5;
constexpr char CS_PARTY_LEAVE			= 6;

constexpr char CS_QUEST_INVENTORY		= 7;
constexpr char CS_SAVE_INVENTORY		= 8;
constexpr char CS_LOGOUT				= 9;

constexpr char SC_LOGIN_FAIL			= 10;
constexpr char SC_LOGIN_SUCCESS			= 11;
constexpr char SC_LOGIN_INFO			= 12;

constexpr char SC_PARTY_LIST_INFO		= 13;
constexpr char SC_PARTY_INFO			= 14;
constexpr char SC_PARTY_STAFF_READY		= 15;
constexpr char SC_PARTY_START			= 16;
constexpr char SC_PARTY_JOIN_FAIL		= 17;
constexpr char SC_PARTY_JOIN_SUCCESS	= 18;
constexpr char SC_PARTY_LEAVE_FAIL		= 19;
constexpr char SC_PARTY_LEAVE_SUCCESS	= 20;

constexpr char SC_LOGOUT_FAIL			= 21;
constexpr char SC_LOGOUT_SUCCESS		= 22;
constexpr char SC_ITEM_INFO				= 23;

constexpr char CS_AWS_TOKEN = 24;


#pragma pack (push, 1)

//struct CS_AWS_TOKEN_PACK {
//	char size;
//	char type;
//	char Token[110];
//	char Token_size;
//};

struct CS_LOGIN_PACK {
	char size;
	char type;

	char Token[110];
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

struct CS_MOVE_PACK {
	char size;
	char type;

	short x;
	short y;
	short z;
};

struct CS_PARTY_SEARCHING_PACK {
	char size;
	char type;
};

struct CS_PARTY_INFO_PACK {
	char size;
	char type;

	char party_num;			// client에서 선택한 파티번호(UI번호)
};

struct CS_PARTY_READY_PACK {
	char size;
	char type;
};

struct CS_PARTY_JOIN_PACK {
	char size;
	char type;

	char party_num;
};

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
	char _pet_num;
	
	char _q_item;
	char _q_skill[4];
};

struct SC_PARTIES_INFO_PACK {	// 파티의 요약정보
	char size;
	char type;

	char _name[CHAR_SIZE];		// 파티의 이름
	char _staff_count;			// 파티원 수
};

struct SC_PARTY_INFO_PACK {		// 각 파티원의 세부 정보
	char size;
	char type;

	char _mem[CHAR_SIZE];
	char _mem_pet;
	char _mem_state;
};

struct SC_PARTY_JOIN_FAIL_PACK {
	char size;
	char type;
};

struct SC_PARTY_JOIN_SUCCESS_PACK {
	char size;
	char type;
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
};

struct SC_LOGOUT_FAIL_PACK {
	char size;
	char type;
};

struct SC_LOGOUT_SUCCESS_PACK {
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