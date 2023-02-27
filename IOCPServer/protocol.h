#pragma once

constexpr int MAX_USER = 100;
constexpr int NAME_SIZE = 30;
constexpr int BUF_SIZE = 128;
constexpr int PORT_NUM = 7777;

constexpr short CHAR_SIZE = 20;

constexpr char CS_LOGIN			= 0;
constexpr char SC_LOGIN_FAIL	= 1;
constexpr char SC_LOGIN_SUCCESS = 2;
constexpr char SC_LOGIN_INFO	= 3;

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