#pragma once

constexpr int MAX_USER				= 100;
constexpr int MAX_PARTY				= 100;
constexpr int BUF_SIZE				= 128;
constexpr int PORT_NUM				= 7777;

constexpr short CHAR_SIZE			= 20;

constexpr char CS_LOGIN				= 0;
constexpr char CS_MOVE				= 1;
constexpr char CS_SEARCHING_PARTY	= 2;
constexpr char CS_PARTY_INFO		= 3;
constexpr char CS_LOGOUT			= 9;

constexpr char SC_LOGIN_FAIL		= 10;
constexpr char SC_LOGIN_SUCCESS		= 11;
constexpr char SC_LOGIN_INFO		= 12;
constexpr char SC_PARTY_LIST_INFO   = 13;
constexpr char SC_PARTY_INFO		= 14;



#pragma pack (push, 1)

struct CS_LOGIN_PACK {
	unsigned char size;
	unsigned char type;
	unsigned char id[CHAR_SIZE];
	unsigned char pw[CHAR_SIZE];
};

struct CS_MOVE_PACK {
	unsigned char size;
	unsigned char type;

	short x;
	short y;
	short z;
};

struct CS_SEARCHING_PARTY_PACK {
	unsigned char size;
	unsigned char type;
};

struct CS_PARTY_INFO_PACK {
	unsigned char size;
	unsigned char type;

	char party_num;
};

struct SC_LOGIN_FAIL_PACK {
	unsigned char size;
	unsigned char type;
};

struct SC_LOGIN_SUCCESS_PACK {
	unsigned char size;
	unsigned char type;
};

struct SC_LOGIN_INFO_PACK {
	unsigned char size;
	unsigned char type;
	char name[CHAR_SIZE];
	
	short x;
	short y;
	short z;
	char direction;
	// character ability and skin ...
	// player item ...

	char _pet_num;
	// pet ability
};

struct SC_MOVE_PACK {
	unsigned char size;
	unsigned char type;

	short x;
	short y;
	short z;
};

struct SC_PARTY_LIST_INFO_PACK {
	unsigned char size;
	unsigned char type;

	char _name[CHAR_SIZE];	// 파티 이름
	char _staff_member;		// 파티인원 (1~4명)
};

struct SC_PARTY_INFO_PACK {
	unsigned char size;
	unsigned char type;

	char _mem[4][CHAR_SIZE];	// 비어있어도 사용
	char _mem_pet[4];
};

#pragma pack (pop)