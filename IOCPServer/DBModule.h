#pragma once

#include "netModule.h"

#include <sqlext.h>

int get_player_uid();

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

bool Login_UDB(short& in_uid, std::string& in_name);
bool Logout_UDB(short in_c_uid);

std::string Get_ItemID(short item_ID, bool full_name);

bool Get_IDB(short in_c_uid);
bool Set_IDB(short in_c_uid);

bool write_DB(int game_id, int pos_x, int pos_y);