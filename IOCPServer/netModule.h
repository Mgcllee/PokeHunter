/*
[Refactoring]
	1. O.O.D
	2. update SESSION Design
*/

#pragma once

#include "stdafx.h"
#include "protocol.h"

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <map>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

enum TYPE { ACCEPT, RECV, SEND, LOGOUT };
enum CLIENT_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
enum PLAYER_STATE { ST_HOME, ST_NOTREADY, ST_READY, ST_STAGE };

class OVER_EXP {
public:
	WSAOVERLAPPED	_over;
	TYPE			c_type;
	WSABUF			_wsabuf;
	char			_send_buf[BUF_SIZE];

	OVER_EXP()
	{
		ZeroMemory(&_over, sizeof(_over));
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		c_type = RECV;
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		c_type = SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

class SESSION {
public:
	OVER_EXP _recv_over;
	SOCKET _socket = NULL;
	int _prev_size;	// 재조립에서 사용


	char _name[CHAR_SIZE];
	short _uid = -1;		// 서버용 플레이어 고유 ID	
	char _pet_num[CHAR_SIZE];
	char _player_skin;

	char Collection[9];
	char Install[9];
	char Launcher[9];
	char Potion[9];
	
	std::map<std::string, short> itemData;

	char storageLauncher[9];
	char storageInstall[9];
	char storagePotion[9];
	char storageCollection[9];

	char _q_item;
	char _q_skill[CHAR_SIZE];

	char _party_num = -1;		// 파티 고유 번호
	char _party_staff_num;	// 파티 내 멤버 번호

	std::string IdToken;
	short IdTokenLenght;

	CLIENT_STATE _state;
	std::mutex _lock;

	PLAYER_STATE _player_state;

	SESSION() {
		_socket = NULL;
		_uid = -1;
		strncpy_s(_name, "None", strlen("None"));
		_prev_size = 0;
		strncpy_s(_pet_num, "NonePartner", strlen("NonePartner"));
		_state = ST_FREE;

		itemData.clear();
		/*for (int i = 0; i < MAX_ITEM_CATEGORY * MAX_ITEM_COUNT; ++i) {
			itemData.insert({ "None", 0 });
		}*/
	}

	SESSION& operator=(SESSION& ref) {
		strncpy_s(this->_name, ref.get_name(), strlen(ref.get_name()));
		this->_socket = ref._socket;
		this->_prev_size = ref._prev_size;

		this->_uid = ref._uid;
		strncpy_s(this->_pet_num, ref._pet_num, strlen(ref._pet_num));
		this->_player_skin = ref._player_skin;

		strncpy_s(this->Collection, ref.Collection, strlen(ref.Collection));
		strncpy_s(this->Install, ref.Install, strlen(ref.Install));
		strncpy_s(this->Launcher, ref.Launcher, strlen(ref.Launcher));
		strncpy_s(this->Potion, ref.Potion, strlen(ref.Potion));

		this->_q_item = ref._q_item;
		strncpy_s(this->_q_skill, ref._q_skill, strlen(ref._q_skill));

		this->_state = ref._state;
		this->_player_state = ref._player_state;

		return *this;
	}

	void clear() {
		_socket = 0;
		ZeroMemory(_name, sizeof(_name));
		IdToken.clear();
		IdTokenLenght = NULL;
		// _recv_over = NULL;
		_prev_size = NULL;
		
		_uid = -1;

		// strncpy_s(_name, "Empty", strlen("Empty")); //-> Unreal Engine의 TArray에서 FName으로 받아 제거해야 함.
		_prev_size = 0;
		strncpy_s(_pet_num, "NonePartner", strlen("NonePartner"));
		{
			std::lock_guard<std::mutex> ll(_lock);
			_state = ST_FREE;
		}

		ZeroMemory(Collection, sizeof(Collection));
		ZeroMemory(Install, sizeof(Install));
		ZeroMemory(Launcher, sizeof(Launcher));
		ZeroMemory(Potion, sizeof(Potion));

		itemData.clear();

		ZeroMemory(storageCollection, sizeof(storageCollection));
		ZeroMemory(storageInstall, sizeof(storageInstall));
		ZeroMemory(storageLauncher, sizeof(storageLauncher));
		ZeroMemory(storagePotion, sizeof(storagePotion));

		_player_skin = 1;

		_q_item = NULL;
		ZeroMemory(_q_skill, sizeof(_q_skill));

		_party_num = -1;		// 파티 고유 번호
		_party_staff_num = NULL;// 파티 내 멤버 번호
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_size;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_size;
		_recv_over.c_type = RECV;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
	}
	void do_send(void* packet)
	{
		OVER_EXP* send_over = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &send_over->_wsabuf, 1, 0, 0, &send_over->_over, 0);
	}



	char* get_name() {
		return _name;
	}
	void set_name(const char* in) {
		strncpy_s(_name, CHAR_SIZE, in, sizeof(in));
	}

	void set_item(const char* in_item_name, short index, char cnt) {
		if (0 == strcmp(in_item_name, "CT")) {
			Collection[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "IS")) {
			Install[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "LC")) {
			Launcher[index] = cnt;
		}
		else if (0 == strcmp(in_item_name, "PT")) {
			Potion[index] = cnt;
		}
	}

	char* get_item_arrayName(short num)
	{
		switch(num)
		{
		case 0:
			return Collection;
			break;
		case 1:
			return Install;
			break;
		case 2:
			return Launcher;
			break;
		case 3:
			return Potion;
			break;
		}
		return nullptr;
	}

	char* get_storage_item_arrayName(short num)
	{
		switch (num)
		{
		case 0:
			return storageCollection;
			break;
		case 1:
			return storageInstall;
			break;
		case 2:
			return storageLauncher;
			break;
		case 3:
			return storagePotion;
			break;
		}
		return nullptr;
	}

	char get_item_cnt(char* category_name, char* item_name)
	{
		if (0 == strcmp("LC", category_name)) {
			if (0 == strcmp("Bullet", item_name)) {
				return Launcher[0];
			}
			else if (0 == strcmp("FireBullet", item_name)) {
				return Launcher[1];
			}
			else if (0 == strcmp("IceBullet", item_name)) {
				return Launcher[2];
			}
			else if (0 == strcmp("ExplosionBullet", item_name)) {
				return Launcher[3];
			}
		}
		else if (0 == strcmp("IS", category_name)) {
			if (0 == strcmp("Trap", item_name)) {
				return Install[0];
			}
			else if (0 == strcmp("DummyTrap", item_name)) {
				return Install[1];
			}
			else if (0 == strcmp("BindTrap", item_name)) {
				return Install[2];
			}
			else if (0 == strcmp("HealTrap", item_name)) {
				return Install[3];
			}
		}
		else if (0 == strcmp("PT", category_name)) {
			if (0 == strcmp("Potion", item_name)) {
				return Potion[0];
			}
		}
		else if (0 == strcmp("CT", category_name)) {

		}
	}
};

/*
default sep: camel
imp sys var : _~
*/
class BASE_SESSION {
	SOCKET _socket;

public:
	BASE_SESSION() : _socket(NULL) { }
	~BASE_SESSION() { }

	bool recv()
	{
		
		return false;
	}

	bool send(void* packet)
	{
		
		return false;
	}
};

// 상속 속성 확인 필요
class CHARACTER : public BASE_SESSION {
	std::string name;

	// Physics
	int x, y, z;

public:
	CHARACTER(int in_x, int in_y, int in_z) 
		: x(in_x), y(in_y), z(in_z)
	{ }

	std::string getName() { return name; }
	void setName(std::string ref_name) {
		// 성능, 보안
		if (name[0] != NULL)
			ref_name = name;
		else
			ref_name = "empty";
	}

};

class HUNTER : public CHARACTER {
	
public:
	HUNTER(std::string hunterName) 
	{ 
		setName(hunterName);
	}

};

class ENEMY : public CHARACTER {


protected:
	int attack_range;

public:

};

class ATTACK_ENEMY : public ENEMY {

public:
	ATTACK_ENEMY()
	{
		setName("aEnemy");
	}

};

class LONG_ATTACK_ENEMY : public ENEMY {

public:
	LONG_ATTACK_ENEMY()
	{
		setName("laEnemy");
	}

};



class PARTY {
public:
	char _name[CHAR_SIZE];
	short _mem_count = 0;
	std::array<SESSION, 4> member;
	bool _inStage;

	PARTY() {
		strncpy_s(_name, CHAR_SIZE, "Empty", strlen("Empty"));
		_mem_count = 0;
		for (SESSION& cl : member) {
			cl._uid = -1;
			strncpy_s(cl._name, CHAR_SIZE, "None", strlen("None"));
		}
		_inStage = false;
	}
	~PARTY() {

	}

	short get_member_count() {
		return _mem_count;
	}

	bool new_member(SESSION& new_mem) {
		if (_mem_count >= 4) return false;

		for (SESSION& mem : member) {
			if (0 == strcmp(mem.get_name(), "None")) {
				mem = new_mem;
				_mem_count += 1;
				return true;
			}
		}
		
		// leave_member()가 작동오류를 했거나, 초기화가 문제임
		return false;
	}

	bool leave_member(char* mem_name) {
		if (_mem_count <= 0) return false;
		for (int i = 0; i < 4; ++i) {
			if (0 == strcmp(member[i].get_name(), mem_name)) {
				for (int j = i; j < 3; ++j) {
					{
						std::lock_guard<std::mutex> ll{ member[j]._lock };
						member[j] = member[j + 1];
					}
					member[member.size() - 1].clear();
				}

				_mem_count -= 1;
				if (0 == _mem_count) {
					_inStage = false;
				}

				return true;
			}
		}
		return false;
	}
};

extern OVER_EXP g_a_over;
extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

extern std::array<SESSION, MAX_USER> clients;	// 플레이어's 컨테이너
extern std::array<PARTY, MAX_PARTY> parties;

void worker_thread(HANDLE h_iocp);
void process_packet(int c_uid, char* packet);

void disconnect(int c_uid);