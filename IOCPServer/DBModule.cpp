#pragma once

#include "DBModule.h"

#include <atlstr.h>


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
			return "NULL";
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
			return "NULL";
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
			return "NULL";
			break;
		}
		break;
	case 3:
		switch (item_ID) {
		case 0:
			return "Potion";
			break;
		default:
			return "NULL";
			break;
		}
		break;
	}
	return "NULL";
}

std::string get_invenDB_SQL(int index) {
	std::string SQL_Order;
	switch (index) {
	case 0:
		// SQL_Order = "SELECT * FROM USERinventoryDB_Collection";
		SQL_Order = "SELECT * FROM USERinventoryDB_Collection";
		break;
	case 1:
		// SQL_Order = "SELECT Trap, DummyTrap, BindTrap, HealTrap FROM USERinventoryDB_Install";
		SQL_Order = "SELECT * FROM USERinventoryDB_Install";
		break;
	case 2:
		// SQL_Order = "SELECT Bullet, FireBullet, IceBullet, ExplosionBullet FROM USERinventoryDB_Launcher";
		SQL_Order = "SELECT * FROM USERinventoryDB_Launcher";
		break;
	case 3:
		// SQL_Order = "SELECT Potion FROM USERinventoryDB_Potion";
		SQL_Order = "SELECT * FROM USERinventoryDB_Potion";
		break;
	}
	return SQL_Order;
}
std::string set_invenDB_SQL(int index, char itemArray[]) {
	"UPDATE userInfo SET PLAYER_SKIN = '1', PLAYER_PET = '2', QUICK_ITEM = '1', QUICK_SKILL='2' WHERE NAME='mgcllee'";

	std::string SQL_Order;
	switch (index) {
	case 0:
		SQL_Order = "UPDATE USERinventoryDB_Collection SET ";
		SQL_Order.append("Ember ='" + itemArray[0]);
		SQL_Order.append("', Crystal ='" + itemArray[1]);
		SQL_Order.append("', Bud ='" + itemArray[2]);
		SQL_Order.append("'");
		break;
	case 1:
		SQL_Order = "UPDATE USERinventoryDB_Install SET ";
		SQL_Order.append("Trap ='"			+ itemArray[0]);
		SQL_Order.append("', DummyTrap ='"	+ itemArray[1]);
		SQL_Order.append("', BindTrap ='"	+ itemArray[2]);
		SQL_Order.append("', HealTrap ='"	+ itemArray[3]);
		SQL_Order.append("'");
		break;
	case 2:
		SQL_Order = "UPDATE USERinventoryDB_Launcher SET ";
		SQL_Order.append("Rock	 ='"			+ itemArray[0]);
		SQL_Order.append("', FireBullet ='"		+ itemArray[1]);
		SQL_Order.append("', IceBullet ='"		+ itemArray[2]);
		SQL_Order.append("', ExplosionBullet ='"+ itemArray[3]);
		SQL_Order.append("'");
		break;
	case 3:
		SQL_Order = "UPDATE USERinventoryDB_Potion SET ";
		SQL_Order.append("Potion ='" + itemArray[0]);
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
		SQL_Order.append("', '10', '10', '10', '10', '0', '0', '0', '0', '0');");
		break;
	case 2:
		SQL_Order = "INSERT INTO USERinventoryDB_Launcher VALUES ('";
		SQL_Order.append(in_name);
		SQL_Order.append("', '99', '10', '10', '10', '0', '0', '0', '0', '0');");
		break;
	case 3:
		SQL_Order = "INSERT INTO USERinventoryDB_Potion VALUES ('";
		SQL_Order.append(in_name);
		SQL_Order.append("', '10', '0', '0', '0', '0', '0', '0', '0', '0');");
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
	SQL_Order.append("', '1', '1', '1', '1111');");

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
						std::cout << clients[c_uid]._name << " New player information Insert SQL 실행 완료\n";
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
		std::cout << "===================================================================" << std::endl;
		std::cout << "[Set New Item]: " << i << std::endl;
		reVal = SetNew_IDB(c_uid, setNew_invenDB_SQL(i, in_name));
		std::cout << "===================================================================" << std::endl;
	}
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
							std::cout << clients[c_uid]._name << " 의 New Item Database SQL 실행 완료\n";
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

	std::cout << "Try to Login!\n";

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

									std::cout << "[New Client!]\n" << "Name: " << clients[in_uid]._name << "\n"
										<< "Player Skin: " << clients[in_uid]._player_skin << "\n"
										<< "Pet Number: " << clients[in_uid]._pet_num << "\n"
										<< "Quick item: " << clients[in_uid]._q_item << "\n"
										<< "Quick skill: " << clients[in_uid]._q_skill << "\n";
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
						std::cout << clients[c_uid]._name << " player Logout SQL 실행 완료\n";
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

bool Get_ALL_ItemDB(int& c_uid) {
	bool reVal = false;

	for (int i = 0; i < MAX_ITEM_CATEGORY; ++i) {
		std::cout << "===================================================================" << std::endl;
		std::cout << "[Item Category]: " << i << std::endl;
		reVal = Get_IDB(c_uid, clients[c_uid].get_item_arrayName(i), get_invenDB_SQL(i));
		std::cout << "===================================================================" << std::endl;
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

	std::cout << SQL_Order << std::endl;

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
										std::cout << "Char: " << db_itemCnt << "\tAtoi: " << atoi(db_itemCnt) << std::endl;

										itemArray[j] = (char)atoi(db_itemCnt);
									}

									std::cout << "Get Player inventory item\n";
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
		std::cout << "===================================================================" << std::endl;
		std::cout << "[Item Category]: " << i << std::endl;
		reVal = Set_IDB(c_uid, set_invenDB_SQL(i, clients[c_uid].get_item_arrayName(i)));
		std::cout << "===================================================================" << std::endl;
	}
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

	std::cout << SQL_Order << std::endl;

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

					for(int i = 0; i < MAX_ITEM_CATEGORY; ++i)
					{
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)(wideCStr), SQL_NTS);

						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							std::cout << clients[c_uid]._name << " 의 Item Database save SQL 실행 완료\n";
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
		std::cout << "===================================================================" << std::endl;
		std::cout << "[Storage Category]: " << i << std::endl;
		// reVal = Get_StorageDB(c_uid, clients[c_uid].get_storage_item_arrayName(i), get_storageDB_SQL(i));
		Get_StorageDB(c_uid, clients[c_uid].get_storage_item_arrayName(i), get_storageDB_SQL(i));
		std::cout << "===================================================================" << std::endl;
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

	std::cout << SQL_Order << std::endl;

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

										std::cout << "Char: " << db_itemCnt << "\tAtoi: " << atoi(db_itemCnt) << std::endl;

										storageArray[j] = (char)atoi(db_itemCnt);
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