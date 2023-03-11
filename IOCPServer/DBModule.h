#pragma once

#include "netModule.h"

#include <sqlext.h>

int get_player_uid();

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

bool Login_UDB(char* in_id, char* in_pass, short& in_c_uid, char* in_name, char& in_skin, char& in_pet, char& in_quickItem, char* in_quickSkill);

bool write_DB(int game_id, int pos_x, int pos_y);