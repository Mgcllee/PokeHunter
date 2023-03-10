#pragma once

#include "netModule.h"

#include <sqlext.h>

int get_player_uid();

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

bool Login_UDB(char* id, char* pass, short& c_uid, char* db_name);

bool write_DB(int game_id, int pos_x, int pos_y);