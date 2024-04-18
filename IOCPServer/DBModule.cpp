#pragma once

#include "DBModule.h"
#include "netModule.h"

#include <atlstr.h>

USER_DB_MANAGER::USER_DB_MANAGER(
	std::string DB_HOST
	, std::string DB_USER
	, std::string DB_PASS
	, std::string DB_NAME
)
{
	user_id = 0;
	printf("%s\n", mysql_get_client_info());

	mysql_init(&connection);
	conn = mysql_real_connect(
		&connection, 
		DB_HOST.c_str(), DB_USER.c_str(), DB_PASS.c_str(), DB_NAME.c_str(),
		3306,			// port_number
		(char*)NULL,	// unix_socket
		0);				// client_flag
}
USER_DB_MANAGER::~USER_DB_MANAGER()
{
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

	char sql[1024];
	char str1[1024], str2[1024];

	strcpy_s(sql, "SELECT Name, Continent FROM world.country");
	if (mysql_query(conn, sql) == 0) {
		result = mysql_store_result(conn);
		while ((row = mysql_fetch_row(result)) != NULL)
		{
			strcpy_s(str1, row[0]);
			strcpy_s(str2, row[1]);
			std::cout << str1 << " : " << str2 << "\n";
		}
		mysql_free_result(result);
	}
	else {
		std::cout << "fail!";
	}

	mysql_close(conn);
	return;
}
std::string USER_DB_MANAGER::process_sql(std::string sql)
{
	try {
		MYSQL_RES* result;
		MYSQL_ROW row;

		if (mysql_query(conn, sql.c_str()) == 0) {
			result = mysql_store_result(conn);
			while ((row = mysql_fetch_row(result)) != NULL)
			{
				row[0];
				// row[1];
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
