#pragma once

#include "netModule.h"

#include <sqlext.h>

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

bool checking_DB(char* p_name, short& c_uid) {
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;
	SQLRETURN retcode;
	SQLWCHAR Name[CHAR_SIZE];
	SQLINTEGER PosX, PosY, EXP;
	SQLLEN cbName = 0, cb_pos_x = 0, cb_pos_y = 0, cb_exp = 0;
	setlocale(LC_ALL, "Korean");

	if (-1 == c_uid) {
		c_uid = get_player_uid();
	}

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"GSPDB", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
					retcode = SQLExecDirect(hstmt, (SQLWCHAR*)L"SELECT NAME, PosX, PosY, EXP FROM PlayerInfo", SQL_NTS);

					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

						retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, Name, CHAR_SIZE, &cbName);
						retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &PosX, 4, &cb_pos_x);
						retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &PosY, 4, &cb_pos_y);
						retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &EXP, 4, &cb_exp);

						for (int i = 0; ; ++i) {
							retcode = SQLFetch(hstmt);
							if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
								// error 검출기
								show_error(hstmt, SQL_HANDLE_STMT, retcode);
							}

							if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
							{
								char db_buf[CHAR_SIZE];
								int strSize = WideCharToMultiByte(CP_ACP, 0, Name, -1, NULL, 0, NULL, NULL);
								WideCharToMultiByte(CP_ACP, 0, Name, -1, db_buf, strSize, 0, 0);

								string c_buf = db_buf;
								c_buf.erase(remove(c_buf.begin(), c_buf.end(), ' '), c_buf.end());

								if (strncmp(c_buf.c_str(), p_name, c_buf.length()) == 0) {
									/*clients[c_uid].x = PosX;
									clients[c_uid].y = PosY;
									strncpy_s(clients[c_id]._name, c_buf.c_str(), c_buf.length());
									clients[c_uid].exp = EXP;*/
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