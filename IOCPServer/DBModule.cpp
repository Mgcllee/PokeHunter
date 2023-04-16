#pragma once

#include "DBModule.h"


int get_player_uid() {
	for (int i = 0; i < MAX_USER; ++i) {
		std::lock_guard <std::mutex> ll{ clients[i]._lock };
		if (clients[i]._socket == NULL)
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
	if (false != full_name) {
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

bool Login_UDB(short& in_uid, std::string& in_name) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQLWCHAR player_skin[CHAR_MIN_SIZE];
	SQLWCHAR player_pet[CHAR_MIN_SIZE];
	SQLWCHAR quick_item[CHAR_MIN_SIZE];
	SQLWCHAR quick_skill[CHAR_MIN_SIZE];

	SQLLEN sqllen{};

	char db_name_buf[CHAR_SIZE];	// char size: CHAR_SIZE
	char db_skin[CHAR_MIN_SIZE], db_pet[CHAR_MIN_SIZE], db_item[CHAR_MIN_SIZE];	// char size: CAHR_MIN_SIZE
	char db_skill[CHAR_MIN_SIZE];															// char size: 4
	int strSize;

	std::string SQL_Order = "SELECT PLAYER_SKIN, PLAYER_PET, QUICK_ITEM, QUICK_SKILL FROM userInfo WHERE NAME='";
	SQL_Order.append(" FROM " + Get_ItemID(0, true));
	SQL_Order.append(" ");
	SQL_Order + in_name + "'";

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
						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, &player_skin, CHAR_MIN_SIZE, &sqllen);
						retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, &player_pet, CHAR_MIN_SIZE, &sqllen);
						retcode = SQLBindCol(hstmt, 3, SQL_C_WCHAR, &quick_item, CHAR_MIN_SIZE, &sqllen);
						retcode = SQLBindCol(hstmt, 4, SQL_C_WCHAR, &quick_skill, CHAR_MIN_SIZE, &sqllen);

						for (int i = 0; ; ++i) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
								// error 검출기
								show_error(hstmt, SQL_HANDLE_STMT, retcode);
							}

							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
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

								if (0 == strcmp(clients[in_uid]._name, c_name_buf.c_str())) {
									strncpy_s(clients[in_uid]._name, CHAR_SIZE, c_name_buf.c_str(), c_name_buf.size());
									clients[in_uid]._player_skin = db_skin[0];
									clients[in_uid]._pet_num = db_pet[0];
									clients[in_uid]._q_item = db_item[0];
									strncpy_s(clients[in_uid]._q_skill, 4, db_skill, 4);

									std::cout << "[New Client!]\n" << "Name: " << clients[in_uid]._name << "\n"
										<< "Player Skin: " << clients[in_uid]._player_skin << "\n"
										<< "Pet Number: " << clients[in_uid]._pet_num << "\n"
										<< "Quick item: " << clients[in_uid]._q_item << "\n"
										<< "Quick skill: " << clients[in_uid]._q_skill << "\n";
									return true;
								}
								else
									return false;
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
bool Logout_UDB(short& c_uid)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	std::string SQL_Order = std::string("UPDATE userInfo SET PLAYER_SKIN='").append(&clients[c_uid]._player_skin)
						.append("', PLAYER_PET='").append(&clients[c_uid]._pet_num)
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

bool Get_IDB(short& c_uid) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR ItemCnt[MAX_ITEM_COUNT];
	SQLLEN sqllen{};

	char db_itemCnt[CHAR_MIN_SIZE];
	int strSize;

	std::string SQL_Order = "SELECT Bullet, FireBullet, IceBullet, ExplosionBullet FROM USERinventoryDB_Launcher WHERE NAME='";
	SQL_Order + clients[c_uid]._name + "'";

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
											clients[c_uid].set_item(Get_ItemID(item_index, false).c_str(), (char)i, (short)ItemCnt[i]);
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
bool Set_IDB(short& c_uid)
{
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	std::string SQL_Order = "UPDATE userInfo SET";
	for (int i = 0; i < MAX_ITEM_CATEGORY; ++i) {
		SQL_Order.append(" <here item name(use index)>='").append("<item cnt>',");
	}
	SQL_Order.append("\b WHERE NAME='").append(clients[c_uid]._name).append("'");

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

bool Get_SDB(short& c_uid) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLLEN sqllen{};

	SQLWCHAR ItemCnt[MAX_ITEM_COUNT];

	char db_itemCnt[CHAR_MIN_SIZE];
	int strSize;

	std::string SQL_Order = "SELECT Bullet, FireBullet, IceBullet, ExplosionBullet FROM USERinventoryDB_Launcher WHERE NAME='";
	SQL_Order + clients[c_uid]._name + "'";

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
											clients[c_uid].set_item(Get_ItemID(item_index, false).c_str(), (char)i, (short)ItemCnt[i]);
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
bool Set_SDB(short& c_uid) {

}