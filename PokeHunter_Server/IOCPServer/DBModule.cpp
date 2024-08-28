#pragma once

#include "DBModule.h"
#include "netModule.h"

#include <atlstr.h>

USER_DB_MANAGER::USER_DB_MANAGER(
	const char* DB_HOST
	, const char* DB_USER
	, const char* DB_PASS
	, const char* DB_NAME
)
{
	user_id = 0;
	printf("%s\n", mysql_get_client_info());

	mysql_init(&connection);
	conn = mysql_real_connect(
		&connection, 
		DB_HOST, DB_USER, DB_PASS, DB_NAME,
		3306,			// port_number
		(char*)NULL,	// unix_socket
		0);				// client_flag
}
USER_DB_MANAGER::~USER_DB_MANAGER()
{
	mysql_close(conn);

	for (PARTY& party : parties)
	{
		party.~PARTY();
	}
	for (PLAYER& player : clients)
	{
		player.get_session()->disconnect();
		player.~PLAYER();
	}

	user_id = 1;
}
void USER_DB_MANAGER::test_mysql_function()
{
	MYSQL_RES* result;
	MYSQL_ROW row;

	process_sql("SELECT GROUP_CONCAT(col_name separator ', ') as name FROM(SELECT COLUMN_NAME as col_name FROM INFORMATION_SCHEMA.COLUMNS WHERE  TABLE_NAME = 'user_info_table') as field_name;");
	process_sql("SELECT * FROM user_info_db.user_info_table");
	
	return;
}
std::string USER_DB_MANAGER::process_sql(std::string sql)
{
	try {
		MYSQL_RES* result;
		MYSQL_ROW row;

		if (mysql_query(conn, sql.c_str()) == 0) {
			result = mysql_store_result(conn);
			int loop_cnt = mysql_num_fields(result);

			while ((row = mysql_fetch_row(result)) != NULL)
			{
				for (int i = 0; i < loop_cnt; ++i)
				{
					printf("%s\t", row[i]);
				}	printf("\n");
			}
			mysql_free_result(result);
		}
		else 
			throw SQL_ERROR_MSG((char*)mysql_error(conn));
	}
	catch (SQL_ERROR_MSG e)
	{
		printf("[SQL][FAIL]: %s\n", e.GetMessageW());
		return "fail";
	}
	return "success";
}
bool USER_DB_MANAGER::get_login_user_data(int& user_id)
{
	if (0 == user_id) 
		user_id = get_new_uid();

	if (true)	// find user data
	{
		
	}
	else if (init_user_data())
	{

	}

	return false;
}
bool USER_DB_MANAGER::save_logout_user_data(int& user_id)
{
	if (0 != user_id)
	{
		
	}

	return false;
}
bool USER_DB_MANAGER::get_user_data(int& user_id)
{
	if (0 != user_id)
	{

	}

	return false;
}
bool USER_DB_MANAGER::init_user_data()
{
	if ("success" == process_sql("INSERT "))
	{

	}

	return false;
}
