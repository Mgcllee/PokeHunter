#pragma once

#include "netModule.h"

#include <sqlext.h>

int get_player_uid();

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

bool SetNew_UDB(int& c_uid, std::string& in_name);
bool SetNew_ALL_ItemDB(int& c_uid, std::string& in_nam);
bool SetNew_IDB(int& c_uid, std::string SQL_Order);

bool Login_UDB(int& in_uid, std::string& in_name);
bool Logout_UDB(int& in_c_uid);

std::string Get_ItemID(short item_ID, bool full_name);
std::string Get_ItemName(short category, short item_ID);

std::string get_invenDB_SQL(int index);
std::string set_invenDB_SQL(int index, char itemArray[]);

bool Get_ALL_ItemDB(int& c_uid);
bool Get_IDB(int& c_uid, char itemArray[], std::string SQL_Order);
bool Set_ALL_ItemDB(int& c_uid);
bool Set_IDB(int& in_c_uid, std::string SQL_Order);

bool Get_SDB(int& c_uid);
bool Set_SDB(int& c_uid);