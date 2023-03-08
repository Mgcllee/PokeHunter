#pragma once

#include "DBModule.h"

int get_player_uid() {
	for (int i = 0; i < MAX_USER; ++i) {
		if (0 == strcmp(clients[i]._name, "empty")) {
			return i;
		}
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

bool Login_UDB(char* id, char* pass, short& c_uid) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;

	SQLWCHAR NAME[CHAR_SIZE];
	SQLWCHAR ID[CHAR_SIZE];
	SQLWCHAR PASS[CHAR_SIZE];

	SQLINTEGER PET;
	SQLINTEGER T_PLAYER;

	SQLLEN cbName = 0, cbId = 0, cbPass = 0;

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
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT NAME, ID, PASS FROM userInfo", SQL_NTS);

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, NAME, CHAR_SIZE, &cbName);
						retcode = SQLBindCol(hstmt, 2, SQL_C_WCHAR, &ID, 4, &cbId);
						retcode = SQLBindCol(hstmt, 3, SQL_C_WCHAR, &PASS, 4, &cbPass);

						for (int i = 0; ; ++i) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
								// error 검출기
								show_error(hstmt, SQL_HANDLE_STMT, retcode);
							}

							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								char db_name_buf[CHAR_SIZE], db_id_buf[CHAR_SIZE], db_pass_buf[CHAR_SIZE];
								int strSize;

								strSize = WideCharToMultiByte(CP_ACP, 0, NAME, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, NAME, -1, db_name_buf, strSize, 0, 0);

								strSize = WideCharToMultiByte(CP_ACP, 0, ID, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, ID, -1, db_id_buf, strSize, 0, 0);
								
								strSize = WideCharToMultiByte(CP_ACP, 0, PASS, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, PASS, -1, db_pass_buf, strSize, 0, 0);

								string c_name_buf = db_name_buf;
								c_name_buf.erase(remove(c_name_buf.begin(), c_name_buf.end(), ' '), c_name_buf.end());

								string c_id_buf = db_id_buf;
								c_id_buf.erase(remove(c_id_buf.begin(), c_id_buf.end(), ' '), c_id_buf.end());

								string c_pass_buf = db_pass_buf;
								c_pass_buf.erase(remove(c_pass_buf.begin(), c_pass_buf.end(), ' '), c_pass_buf.end());

								if (strncmp(c_id_buf.c_str(), id, c_id_buf.length()) == 0
									&& strncmp(c_pass_buf.c_str(), pass, c_pass_buf.length()) == 0) {

									if (-1 == c_uid) {
										c_uid = get_player_uid();
									}
									else {
										strncpy_s(clients[c_uid]._name, c_name_buf.c_str(), sizeof(c_name_buf));
										
									}
									return true;
								}
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


bool write_DB(int game_id, int pos_x, int pos_y) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR szName[CHAR_SIZE];
	SQLINTEGER user_id, POSITION_X, POSITION_Y;

	SQLLEN cbName = 0, cb_pos_x = 0, cb_pos_y = 0, cb_id = 0;

	setlocale(LC_ALL, "Korean");

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"2022GAMEODBC", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					// retcode = SQLExecDirect(hstmt, (SQLWCHAR*)((L"UPDATE user_table_2018184020 SET POSITION_X = " + std::to_wstring(pos_x) + L" WHERE user_id = " + std::to_wstring(game_id)).c_str()), SQL_NTS);
					// retcode = SQLExecDirect(hstmt, (SQLWCHAR*)((L"UPDATE user_table_2018184020 SET POSITION_Y = " + std::to_wstring(pos_y) + L" WHERE user_id = " + std::to_wstring(game_id)).c_str()), SQL_NTS);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						// cout << "위치 저장 완료" << endl;
					}
					else {
						// cout << "유효하지 않은 ID 입니다." << endl;
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

