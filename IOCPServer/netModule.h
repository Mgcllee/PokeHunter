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
enum SESSION_STATE { ST_FREE, ST_ALLOC };
enum PLAYER_STATE { ST_HOME, ST_NOTREADY, ST_READY, ST_STAGE };

class OVER_EXP;
class SESSION;
class PLAYER;
class PARTY;

extern OVER_EXP g_a_over;
extern HANDLE h_iocp;
extern SOCKET g_s_socket, g_c_socket;

extern std::array<PLAYER, MAX_USER> clients;
extern std::array<PARTY, MAX_PARTY> parties;

void worker_thread(HANDLE h_iocp);

void process_packet(int c_uid, char* packet);

void disconnect(int c_uid);