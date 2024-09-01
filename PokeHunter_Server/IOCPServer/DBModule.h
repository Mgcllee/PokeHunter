#pragma once

#include <mysql.h>
#include <sqlext.h>

#include "netModule.h"
#pragma comment(lib, "libmysql.lib")

class SQL_ERROR_MSG {
 public:
  SQL_ERROR_MSG() : Label("Generic Error") {}
  SQL_ERROR_MSG(char* message) { Label = message; }
  ~SQL_ERROR_MSG() {}

  inline const char* GetMessage(void) { return Label.c_str(); }

 private:
  std::string Label;
};

class USER_DB_MANAGER {
 public:
  USER_DB_MANAGER(const char* DB_HOST, const char* DB_USER, const char* DB_PASS,
                  const char* DB_NAME);
  ~USER_DB_MANAGER();

  void test_mysql_function();

  void _lock() { this->ll.lock(); }
  void _unlock() { this->ll.unlock(); }

  /*
  array���� ������� �ʴ� user_id�� ������ ���,
  ������ �� �ֵ��� ���� �ʿ�.
  */
  int get_new_uid() {
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