#pragma once

#include "DBModule.h"

#include <atlstr.h>
#include <ctime>	// Player Skin을 적용할 때 랜덤시드로 사용


int get_player_uid() {
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard <std::mutex> ll{ clients[i]._lock };
		if ((clients[i]._state == ST_FREE) && (clients[i]._socket == NULL))
			return i;
	}
	return -1;
}

void show_error(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];

	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

std::string Get_ItemID(short item_ID, bool full_name) {
	if (!full_name) {
		switch (item_ID) {
		case 1:
			return "CT";
			break;
		case 2:
			return "IS";
			break;
		case 3:
			return "LC";
			break;
		case 4:
			return "PT";
			break;
		}
	}
	else {
		switch (item_ID) {
		case 1:
			return "USERinventoryDB_Collection";
			break;
		case 2:
			return "USERinventoryDB_Install";
			break;
		case 3:
			return "USERinventoryDB_Launcher";
			break;
		case 4:
			return "USERinventoryDB_Potion";
			break;
		}
	}
	return "Empty";
}
std::string Get_ItemName(short category, short item_ID) {
	switch (category) {
	case 0:
		switch (item_ID) {
		case 0:
			return "Ember";
			break;
		case 1:
			return "Crystal";
			break;
		case 2:
			return "Bud";
			break;
		default:
			return "None";
			break;
		}
		break;
	case 1:
		switch (item_ID) {
		case 0:
			return "Trap";
			break;
		case 1:
			return "DummyTrap";
			break;
		case 2:
			return "BindTrap";
			break;
		case 3:
			return "HealTrap";
			break;
		default:
			return "None";
			break;
		}
		break;
	case 2:
		switch (item_ID) {
		case 0:
			return "Rock";
			break;
		case 1:
			return "FireBullet";
			break;
		case 2:
			return "IceBullet";
			break;
		case 3:
			return "ExplosionBullet";
			break;
		default:
			return "None";
			break;
		}
		break;
	case 3:
		switch (item_ID) {
		case 0:
			return "Potion";
			break;
		default:
			return "None";
			break;
		}
		break;
	}
	return "None";
}

// ~_SQL 함수를 MSDB 기능으로 대체 예정
std::string get_invenDB_SQL(int index) {
	std::string SQL_Order;
	switch (index) {
	case 0:
		SQL_Order = "SELECT * FROM USERinventoryDB_Collection";
		break;
	case 1:
		SQL_Order = "SELECT * FROM USERinventoryDB_Install";
		break;
	case 2:
		SQL_Order = "SELECT * FROM USERinventoryDB_Launcher";
		break;
	case 3:
		SQL_Order = "SELECT * FROM USERinventoryDB_Potion";
		break;
	}
	return SQL_Order;
}
std::string set_invenDB_SQL(int index, char itemArray[]) {
	std::string SQL_Order;
	switch (index) {
	case 0:
		SQL_Order = "UPDATE USERinventoryDB_Collection SET ";
		SQL_Order.append("Ember ='"		+ std::to_string(static_cast<int>(itemArray[0] <= 0 ? 0 : itemArray[0])));
		SQL_Order.append("', Crystal ='"	+ std::to_string(static_cast<int>(itemArray[1] <= 0 ? 0 : itemArray[1])));
		SQL_Order.append("', Bud ='"		+ std::to_string(static_cast<int>(itemArray[2] <= 0 ? 0 : itemArray[2])));
		SQL_Order.append("'");
		break;
	case 1:
		SQL_Order = "UPDATE USERinventoryDB_Install SET ";
		SQL_Order.append("Trap ='"			+ std::to_string(static_cast<int>(itemArray[0] <= 0 ? 0 : itemArray[0])));
		SQL_Order.append("', DummyTrap ='"	+ std::to_string(static_cast<int>(itemArray[1] <= 0 ? 0 : itemArray[1])));
		SQL_Order.append("', BindTrap ='"		+ std::to_string(static_cast<int>(itemArray[2] <= 0 ? 0 : itemArray[2])));
		SQL_Order.append("', HealTrap ='"		+ std::to_string(static_cast<int>(itemArray[3] <= 0 ? 0 : itemArray[3])));
		SQL_Order.append("'");
		break;
	case 2:
		SQL_Order = "UPDATE USERinventoryDB_Launcher SET ";
		SQL_Order.append("Rock	 ='"			+ std::to_string(static_cast<int>(itemArray[0] <= 0 ? 0 : itemArray[0])));
		SQL_Order.append("', FireBullet ='"		+ std::to_string(static_cast<int>(itemArray[1] <= 0 ? 0 : itemArray[1])));
		SQL_Order.append("', IceBullet ='"		+ std::to_string(static_cast<int>(itemArray[2] <= 0 ? 0 : itemArray[2])));
		SQL_Order.append("', ExplosionBullet ='"	+ std::to_string(static_cast<int>(itemArray[3] <= 0 ? 0 : itemArray[3])));
		SQL_Order.append("'");
		break;
	case 3:
		SQL_Order = "UPDATE USERinventoryDB_Potion SET ";
		SQL_Order.append("Potion ='" + std::to_string(static_cast<int>(itemArray[0] <= 0 ? 0 : itemArray[0])));
		SQL_Order.append("'");
		break;
	}
	return SQL_Order;
}
std::string setNew_invenDB_SQL(int index, std::string& in_name) {
	std::string SQL_Order;
	switch (index) {
	case 0:
		SQL_Order = "INSERT INTO USERinventoryDB_Collection VALUES ('";
		SQL_Order.append(in_name);
		SQL_Order.append("', '10', '10', '10', '0', '0', '0', '0', '0', '0');");
		break;
	case 1:
		SQL_Order = "INSERT INTO USERinventoryDB_Install VALUES ('";
		SQL_Order.append(in_name);
		SQL_Order.append("', '0', '0', '0', '0', '0', '0', '0', '0', '0');");
		break;
	case 2:
		SQL_Order = "INSERT INTO USERinventoryDB_Launcher VALUES ('";
		SQL_Order.append(in_name);
		SQL_Order.append("', '99', '10', '10', '10', '0', '0', '0', '0', '0');");
		break;
	case 3:
		SQL_Order = "INSERT INTO USERinventoryDB_Potion VALUES ('";
		SQL_Order.append(in_name);
		SQL_Order.append("', '99', '0', '0', '0', '0', '0', '0', '0', '0');");
		break;
	}
	return SQL_Order;
}
std::string get_storageDB_SQL(int index) {
	std::string SQL_Order;
	switch (index) {
	case 0:
		SQL_Order = "SELECT * FROM USERstorageDB_Collection02";
		break;
	case 1:
		SQL_Order = "SELECT * FROM USERstorageDB_Install";
		break;
	case 2:
		SQL_Order = "SELECT * FROM USERstorageDB_Launcher";
		break;
	case 3:
		SQL_Order = "SELECT * FROM USERstorageDB_Potion";
		break;
	}
	return SQL_Order;
}

// 새로운 유저 Init 을 MSDB로 대체 예정
bool SetNew_UDB(int& c_uid, std::string& in_name) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQLWCHAR player_name[CHAR_SIZE]{};
	SQLWCHAR player_skin[CHAR_SIZE]{};
	SQLWCHAR player_pet[CHAR_SIZE]{};
	SQLWCHAR quick_item[CHAR_SIZE]{};
	SQLWCHAR quick_skill[CHAR_SIZE]{};

	SQLLEN sqllen{};

	char db_name_buf[CHAR_SIZE]{};	// char size: CHAR_SIZE
	char db_skin[CHAR_SIZE]{}, db_pet[CHAR_SIZE]{}, db_item[CHAR_SIZE]{};	// char size: CAHR_MIN_SIZE
	char db_skill[CHAR_SIZE]{};															// char size: 4
	int strSize{};

	std::string SQL_Order = "INSERT INTO userInfo VALUES ('";
	SQL_Order.append(in_name);
	SQL_Order.append("', '");
	SQL_Order.push_back((char)(time(NULL) % 4 + 1 + '0'));	// 플레이어의 랜덤 스킨(1~4)
	SQL_Order.append("', '1', '1', '3333');");

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKEUDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						printf("Complete running of %s's New player information Insert SQL\n", clients[c_uid].get_name());
						return true;
					}
					else {
						show_error(hstmt, SQL_HANDLE_STMT, retcode);
						return false;
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}
					SQLDisconnect(hdbc);
				}
				else {
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return false;
}
bool SetNew_ALL_ItemDB(int& c_uid, std::string& in_name) {
	bool reVal = false;

	for (int i = 0; i < MAX_ITEM_CATEGORY; ++i) {
		reVal = SetNew_IDB(c_uid, setNew_invenDB_SQL(i, in_name));
	}
	printf("Complete running of %s's New Item Database SQL\n", clients[c_uid].get_name());
	return reVal;
}
bool SetNew_IDB(int& c_uid, std::string SQL_Order) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKEIDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					for (int i = 0; i < MAX_ITEM_CATEGORY; ++i)
					{
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							return true;
						}
						else {
							show_error(hstmt, SQL_HANDLE_STMT, retcode);
							return false;
						}
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else {
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return false;
}

bool Login_UDB(int& in_uid, std::string& in_name) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQLWCHAR player_name[CHAR_SIZE];
	SQLWCHAR player_skin[CHAR_SIZE];
	SQLWCHAR player_pet[CHAR_SIZE];
	SQLWCHAR quick_item[CHAR_SIZE];
	SQLWCHAR quick_skill[CHAR_SIZE];

	SQLLEN sqllen{};

	char db_name_buf[CHAR_SIZE];	// char size: CHAR_SIZE
	char db_skin[CHAR_SIZE], db_pet[CHAR_SIZE], db_item[CHAR_SIZE];	// char size: CAHR_MIN_SIZE
	char db_skill[CHAR_SIZE];															// char size: 4
	int strSize;

	std::string SQL_Order = "SELECT * FROM userInfo WHERE NAME='";
	SQL_Order.append(in_name);
	SQL_Order.append("'");

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKEUDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, &player_name, CHAR_SIZE, &sqllen);
						retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, &player_skin, CHAR_SIZE, &sqllen);
						retcode = SQLBindCol(hstmt, 3, SQL_C_WCHAR, &player_pet, CHAR_SIZE, &sqllen);
						retcode = SQLBindCol(hstmt, 4, SQL_C_WCHAR, &quick_item, CHAR_SIZE, &sqllen);
						retcode = SQLBindCol(hstmt, 5, SQL_C_WCHAR, &quick_skill, CHAR_SIZE, &sqllen);

						for (int i = 0; ; ++i) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
								// error 검출기
								show_error(hstmt, SQL_HANDLE_STMT, retcode);
							}

							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								strSize = WideCharToMultiByte(CP_ACP, 0, player_name, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, player_name, -1, db_name_buf, strSize, 0, 0);

								strSize = WideCharToMultiByte(CP_ACP, 0, player_skin, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, player_skin, -1, db_skin, strSize, 0, 0);

								strSize = WideCharToMultiByte(CP_ACP, 0, player_pet, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, player_pet, -1, db_pet, strSize, 0, 0);

								strSize = WideCharToMultiByte(CP_ACP, 0, quick_item, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, quick_item, -1, db_item, strSize, 0, 0);
								
								strSize = WideCharToMultiByte(CP_ACP, 0, quick_skill, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, quick_skill, -1, db_skill, strSize, 0, 0);

								std::string c_name_buf = db_name_buf;
								c_name_buf.erase(remove(c_name_buf.begin(), c_name_buf.end(), ' '), c_name_buf.end());
								in_name.erase(remove(in_name.begin(), in_name.end(), ' '), in_name.end());
								
								if (0 == c_name_buf.compare(in_name)) {
									strncpy_s(clients[in_uid]._name, CHAR_SIZE, c_name_buf.c_str(), strlen(c_name_buf.c_str()));
									clients[in_uid]._player_skin = db_skin[0];
									strncpy_s(clients[in_uid]._pet_num, CHAR_SIZE, db_pet, strlen(db_pet));
									clients[in_uid]._q_item = db_item[0];
									strncpy_s(clients[in_uid]._q_skill, CHAR_SIZE, db_skill, strlen(db_skill));
									/*printf("[%s]\nPlayer Skin: %c\nPet Number: %s\nQuick item: %c\nQuick skill: %c", 
										clients[in_uid]._name, clients[in_uid]._player_skin, clients[in_uid]._pet_num, clients[in_uid]._q_item, clients[in_uid]._q_skill);*/
									return true;
								}
								else {
									show_error(hdbc, SQL_HANDLE_DBC, retcode);
									return false;
								}
							}
							else {
								show_error(hdbc, SQL_HANDLE_DBC, retcode);
								return false;
							}
						}
					}
					else {
						show_error(hstmt, SQL_HANDLE_STMT, retcode);
						return false;
					}

					// Process data
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else {
					// error 검출기
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	show_error(hstmt, SQL_HANDLE_STMT, retcode);
	return false;
}
bool Logout_UDB(int& c_uid)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	std::string SQL_Order = std::string("UPDATE userInfo SET PLAYER_SKIN='").append(&clients[c_uid]._player_skin)
						.append("', PLAYER_PET='").append(clients[c_uid]._pet_num)
						.append("', QUICK_ITEM='").append(&clients[c_uid]._q_item)
						.append("', QUICK_SKILL='").append(clients[c_uid]._q_skill)
						.append("' WHERE NAME='").append(clients[c_uid].get_name()).append("'");
	
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");
	
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
		
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
			
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKEUDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);
  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						printf("Complete running of %s's Player Logout SQL\n", clients[c_uid].get_name());
						return true;
					} 
					else {
						show_error(hstmt, SQL_HANDLE_STMT, retcode);
						return false;
					}

					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}
					SQLDisconnect(hdbc);
				}
				else {
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return false;
}


// SQL을 MSDB에서 직접 실행하여 소스코드 줄이기 필요.
bool Get_ALL_ItemDB(int& c_uid) {
	bool reVal = false;

	for (int i = 0; i < MAX_ITEM_CATEGORY; ++i) {
		reVal = Get_IDB(c_uid, clients[c_uid].get_item_arrayName(i), get_invenDB_SQL(i));
	}
	return reVal;
}
bool Get_IDB(int& c_uid, char itemArray[], std::string SQL_Order) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR ItemCnt[MAX_ITEM_COUNT][CHAR_SIZE];
	SQLLEN sqllen{};

	char db_itemCnt[CHAR_SIZE];
	int strSize;

	SQL_Order.append(" WHERE NAME='");
	SQL_Order.append(clients[c_uid]._name);
	SQL_Order.append("'");

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKEIDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

					for (short item_index = 0; item_index < MAX_ITEM_CATEGORY; ++item_index) {
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							for (int i = 0; i < 9; ++i) {
								retcode = SQLBindCol(hstmt, i + 1, SQL_C_WCHAR, ItemCnt[i], CHAR_SIZE, &sqllen);
							}

							for (int i = 0; ; ++i) {
								retcode = SQLFetch(hstmt);
								if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) show_error(hstmt, SQL_HANDLE_STMT, retcode);

								if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
								{
									for (int j = 1; j <= 9; ++j) {
										// j == 0 : SQL에서 SELECT * 이므로 NAME 까지 함께 가져온다.
										strSize = WideCharToMultiByte(CP_ACP, 0, ItemCnt[j], -1, NULL, 0, NULL, NULL);
										WideCharToMultiByte(CP_ACP, 0, ItemCnt[j], -1, db_itemCnt, strSize, 0, 0);
										if (0 == atoi(db_itemCnt))	break;
										itemArray[j - 1] = (char)atoi(db_itemCnt);
									}
									return true;
								}
								else
									return false;
							}
						}
						else {
							// error 검출기
							show_error(hstmt, SQL_HANDLE_STMT, retcode);
							return false;
						}

						// Process data
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							SQLCancel(hstmt);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
						}

						SQLDisconnect(hdbc);
					}
				}
				else {
					// error 검출기
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return false;
}

bool Set_ALL_ItemDB(int& c_uid) {
	bool reVal = false;

	for (int i = 0; i < MAX_ITEM_CATEGORY; ++i) {
		reVal = Set_IDB(c_uid, set_invenDB_SQL(i, clients[c_uid].get_item_arrayName(i)));
	}
	printf("Complete running of %s Item Database save SQL\n", clients[c_uid].get_name());
	return reVal;
}
bool Set_IDB(int& c_uid, std::string SQL_Order)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQL_Order.append(" WHERE NAME='");
	SQL_Order.append(clients[c_uid]._name);
	SQL_Order.append("'");

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");

	std::cout << SQL_Order << std::endl;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKEIDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					for(int i = 0; i < MAX_ITEM_CATEGORY; ++i)
					{
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							return true;
						}
						else {
							show_error(hstmt, SQL_HANDLE_STMT, retcode);
							return false;
						}
					}
					
					// Process data  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						SQLCancel(hstmt);
						SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					}

					SQLDisconnect(hdbc);
				}
				else {
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return false;
}

bool Get_SDB(int& c_uid) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLLEN sqllen{};

	SQLWCHAR ItemCnt[MAX_ITEM_COUNT];

	char db_itemCnt[CHAR_SIZE];
	int strSize;

	std::string SQL_Order = "SELECT Bullet, FireBullet, IceBullet, ExplosionBullet FROM USERinventoryDB_Launcher WHERE NAME='";
	// SQL_Order + clients[c_uid]._name + "'";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKEIDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

					for (short item_index = 0; item_index < MAX_ITEM_CATEGORY; ++item_index) {
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							for (int i = 0; i < MAX_ITEM_COUNT; ++i) {
								retcode = SQLBindCol(hstmt, i, SQL_C_WCHAR, &ItemCnt[i], 1, &sqllen);
							}

							for (int i = 0; ; ++i) {
								retcode = SQLFetch(hstmt);
								if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) show_error(hstmt, SQL_HANDLE_STMT, retcode);

								if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
								{
									for (int dbi = 0; dbi < MAX_ITEM_COUNT; ++dbi) {
										strSize = WideCharToMultiByte(CP_ACP, 0, &ItemCnt[dbi], -1, NULL, 0, NULL, NULL);
										WideCharToMultiByte(CP_ACP, 0, &ItemCnt[dbi], -1, db_itemCnt, strSize, 0, 0);

										if (0 != ItemCnt[dbi]) {
											clients[c_uid].set_item(Get_ItemID(item_index, false).c_str(), static_cast<char>(i), static_cast<char>(ItemCnt[i]));
										}
									}
									return true;
								}
								else
									return false;
							}
						}
						else {
							// error 검출기
							show_error(hstmt, SQL_HANDLE_STMT, retcode);
							return false;
						}

						// Process data
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							SQLCancel(hstmt);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
						}

						SQLDisconnect(hdbc);
					}
				}
				else {
					// error 검출기
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return false;
}
bool Set_SDB(int& c_uid) {

	return false;
}

bool Get_ALL_StorageDB(int& c_uid) {
	bool reVal = false;

	for (int i = 0; i < MAX_ITEM_CATEGORY; ++i) {
		reVal = Get_StorageDB(c_uid, clients[c_uid].get_storage_item_arrayName(i), get_storageDB_SQL(i));
	}
	return reVal;
}
bool Get_StorageDB(int& c_uid, char storageArray[], std::string SQL_Order) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR ItemCnt[MAX_ITEM_COUNT][CHAR_SIZE]{};
	SQLLEN sqllen{};

	int strSize;

	SQL_Order.append(" WHERE NAME='");
	SQL_Order.append(clients[c_uid]._name);
	SQL_Order.append("'");

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), NULL, 0);
	std::wstring wideStr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &SQL_Order[0], (int)SQL_Order.size(), &wideStr[0], size_needed);
	const wchar_t* wideCStr = wideStr.c_str();

	setlocale(LC_ALL, "Korean");

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"POKESTDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

					for (short item_index = 0; item_index < MAX_ITEM_CATEGORY; ++item_index) {
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							for (int i = 0; i < 9; ++i) {
								retcode = SQLBindCol(hstmt, i + 1, SQL_C_WCHAR, ItemCnt[i], CHAR_SIZE, &sqllen);
							}

							for (int i = 0; ; ++i) {
								retcode = SQLFetch(hstmt);
								if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) show_error(hstmt, SQL_HANDLE_STMT, retcode);

								if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

									// j == 0 : SQL에서 SELECT * 이므로 NAME 까지 함께 가져온다.
									for (int j = 1; j < 9 && 0 != wcscmp(ItemCnt[j], L""); ++j) {
										CW2A cw2a(ItemCnt[j]);
										char db_itemCnt[CHAR_SIZE];
										strcpy_s(db_itemCnt, cw2a);
										storageArray[j - 1] = (char)atoi(db_itemCnt);
									}

									return true;
								}
								else
									return false;
							}
						}
						else {
							show_error(hstmt, SQL_HANDLE_STMT, retcode);
							return false;
						}

						// Process data
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							SQLCancel(hstmt);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
						}

						SQLDisconnect(hdbc);
					}
				}
				else {
					// error 검출기
					show_error(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
	return false;
}