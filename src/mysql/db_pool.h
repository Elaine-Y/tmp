#pragma once

#include <mysql.h>
#include <stdint.h>

#include <condition_variable>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <string>

using std::string;


// 返回结果 select的时候用
class ResultSet {
 public:
  ResultSet(MYSQL_RES* res);
  virtual ~ResultSet();

  bool next();
  int get_int(const string& key);
  string get_string(const string& key);

 private:
  int get_index(const string& key);

  MYSQL_RES* m_res;
  MYSQL_ROW m_row;
  std::map<string, int> m_key_map;
};

class DBPool;

class DBConn {
 public:
  DBConn(DBPool* pDBPool);
  virtual ~DBConn();
  int init();

  // 查询
  ResultSet* do_query(const string& sql);

  // 开启事务
  bool start_tracsaction();
  // 提交事务
  bool commit();
  // 回滚事务
  bool rollback();
  // 获取连接池名
  const string& get_pool_name();
  MYSQL* get_mysql() { return m_mysql; }

 private:
  DBPool* m_dbpool;  // to get MySQL server information
  MYSQL* m_mysql;     // 对应一个连接
  char m_escape_string[MAX_ESCAPE_STRING_LEN + 1];
};

class DBPool {  // 只是负责管理连接DBConn，真正干活的是DBConn
 public:
  DBPool() {}
  DBPool(const string& pool_name, const string& db_server_ip,
         uint16_t db_server_port, const string& username, const string& password,
         const string& db_name, int max_conn_cnt);
  virtual ~DBPool();

  int init();  // 连接数据库，创建连接
  DBConn* get_db_conn(const int timeout_ms = 0);  // 获取连接资源
  void release_db_conn(DBConn* pConn);             // 归还连接资源

  const char* get_pool_name() { return m_pool_name.c_str(); }
  const char* get_db_server_ip() { return m_db_server_ip.c_str(); }
  uint16_t get_db_server_port() { return m_db_server_port; }
  const char* get_user_name() { return m_username.c_str(); }
  const char* get_password() { return m_password.c_str(); }
  const char* get_db_name() { return m_db_name.c_str(); }

 private:
  string m_pool_name;          // 连接池名称
  string m_db_server_ip;       // 数据库ip
  uint16_t m_db_server_port;   // 数据库端口
  string m_username;           // 用户名
  string m_password;           // 用户密码
  string m_db_name;            // db名称
  int m_db_cur_conn_cnt;       // 当前启用的连接数量
  int m_db_max_conn_cnt;       // 最大连接数量
  std::list<DBConn*> m_free_list;  // 空闲的连接

  std::list<DBConn*> m_used_list;  // 记录已经被请求的连接
  std::mutex m_mutex;
  std::condition_variable m_cond_var;
  bool m_abort_request = false;
};