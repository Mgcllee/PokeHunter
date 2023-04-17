#pragma once

#include "netModule.h"

#include <sqlext.h>

int get_player_uid();

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

bool Login_UDB(short& in_uid, std::string& in_name);
bool Logout_UDB(short& in_c_uid);

std::string Get_ItemID(short item_ID, bool full_name);
std::string Get_ItemName(short category, short item_ID);

bool Get_ALL_ItemDB(short& c_uid);
bool Get_IDB(short& c_uid, char* itemArray, std::string SQL_Order);
bool Set_IDB(short& in_c_uid);

bool Get_SDB(short& c_uid);
bool Set_SDB(short& c_uid);