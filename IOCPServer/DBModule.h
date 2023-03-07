#pragma once

#include "netModule.h"

#include <sqlext.h>

int get_player_uid();

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

bool checking_DB(char* p_name, short& c_uid);

bool write_DB(int game_id, int pos_x, int pos_y);