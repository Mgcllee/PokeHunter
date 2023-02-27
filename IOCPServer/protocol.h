#pragma once

constexpr int MAX_USER = 100;
constexpr int NAME_SIZE = 30;
constexpr int BUF_SIZE = 128;
constexpr int PORT_NUM = 7777;

constexpr short CHAR_SIZE = 20;

constexpr char CS_LOGIN			= 0;
constexpr char CS_MOVE			= 1;
constexpr char CS_ATTACK		= 2;
constexpr char CS_LOGOUT		= 3;

constexpr char SC_LOGIN_FAIL	= 10;
constexpr char SC_LOGIN_SUCCESS = 11;
constexpr char SC_LOGIN_INFO	= 12;



#pragma pack (push, 1)

struct CS_LOGIN_PACK {
	unsigned char size;
	unsigned char type;
	unsigned char id[CHAR_SIZE];
	unsigned char pw[CHAR_SIZE];
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
	unsigned char name[CHAR_SIZE];
};

#pragma pack (pop)