#pragma once

#include "netModule.h"

#include <sqlext.h>

#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode){ return; }
bool SetNew_UDB(int& c_uid, std::string& in_name){ return false; }
bool SetNew_ALL_ItemDB(int& c_uid, std::string& in_nam){ return false; }
bool SetNew_IDB(int& c_uid, std::string SQL_Order){ return false; }
bool Login_UDB(int& in_uid, std::string& in_name){ return false; }
bool Logout_UDB(int& in_c_uid){ return false; }
std::string Get_ItemID(short item_ID, bool full_name){ return false; }
std::string Get_ItemName(short category, short item_ID){ return false; }
std::string get_invenDB_SQL(int index){ return false; }
std::string set_invenDB_SQL(int index, char itemArray[]){ return false; }
bool Get_ALL_ItemDB(int& c_uid){ return false; }
bool Get_IDB(int& c_uid, char itemArray[], std::string SQL_Order){ return false; }
bool Set_ALL_ItemDB(int& c_uid){ return false; }
bool Set_IDB(int& in_c_uid, std::string SQL_Order){ return false; }
bool Get_SDB(int& c_uid){ return false; }
bool Set_SDB(int& c_uid){ return false; }
bool Get_ALL_StorageDB(int& c_uid){ return false; }
bool Get_StorageDB(int& c_uid, char storageArray[], std::string SQL_Order){ return false; }

class SQL_ERROR_MSG
{
public:
	SQL_ERROR_MSG() :
		Label("Generic Error")
	{
	}
	SQL_ERROR_MSG(char* message)
	{
		Label = message;
	}
	~SQL_ERROR_MSG() { }

	inline const char* GetMessage(void) { return Label.c_str(); }

private:
	std::string Label;

};

class USER_DB_MANAGER 
{
public:
	USER_DB_MANAGER(
		std::string DB_HOST
		, std::string USER
		, std::string PASS
		, std::string NAME);
	~USER_DB_MANAGER();

	void test_mysql_function();

	void _lock() { this->ll.lock(); }
	void _unlock() { this->ll.unlock(); }

	/*
	array에서 사용하지 않는 user_id가 존재할 경우,
	재사용할 수 있도록 조정 필요.
	*/
	int get_new_uid() 
	{
		ll.lock();
		++user_id;
		ll.unlock();

		return user_id; 
	}

	std::string process_sql(std::string sql);
	bool get_login_user_data(int& user_id);
	bool save_logout_user_data(int& user_id);
	bool get_user_data(int& user_id);

private:
	int user_id;

	std::mutex ll;

	MYSQL* conn;
	MYSQL connection;

	bool init_user_data();
};