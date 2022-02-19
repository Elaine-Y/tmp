#include "mysql_pool.h"

#include <mutex>

ResultSet::ResultSet(MYSQL_RES* res) : m_res(res) {
  uint32_t num_fields = mysql_num_fields(res);
  MYSQL_FIELD* fields = mysql_fetch_fields(m_res);
  for (uint32_t i = 0; i < num_fields; ++i) {
    // 多个字段
    m_key_map.insert(make_pair(fields[i].name, i));
  }
}

ResultSet::~ResultSet() {
  if (m_res) {
    mysql_free_result(m_res);
    m_res = nullptr;
  }
}

bool ResultSet::next() {
  m_row = mysql_fetch_row(m_res);
  if (m_row) {
    return true;
  } else {
    return false;
  }
}

int32_t ResultSet::get_int(const string& key) {
  int32_t idx = get_index(key);
  if (-1 == idx) {
    return 0;
  } else {
    return std::stoi(m_row[idx]);
  }
}

string ResultSet::get_string(const string& key) {
  int32_t idx = get_index(key);
  if (-1 == idx) {
    return nullptr;
  } else {
    return m_row[idx];
  }
}

// 表字段映射到index
int32_t ResultSet::get_index(const string& key) {
  auto it = m_key_map.find(key);
  if (it == m_key_map.end()) {
    return -1;
  } else {
    return it->second;
  }
}

MysqlConn::MysqlConn(DBPool* db_pool) : m_dbpool(db_pool), m_mysql(nullptr) {}

MysqlConn::~MysqlConn() {
  if (m_mysql) {
    mysql_close(m_mysql);
  }
}

bool MysqlConn::init() {
  m_mysql = mysql_init(nullptr);
  if (!m_mysql) {
    std::cout << "[ERR] mysql_init failed" << std::endl;
    return false;
  }
  bool reconnect = true;
  mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &reconnect);  // 自动重连
  mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8mb4");

  if (!mysql_real_connect(m_mysql, m_dbpool->get_db_server_ip(),
                          m_dbpool->get_user_name(), m_dbpool->get_password(),
                          m_dbpool->get_db_name(),
                          m_dbpool->get_db_server_port(), nullptr, 0)) {
    std::cout << "[ERR] mysql_real_connect failed:" << mysql_error(m_mysql)
              << std::endl;
    return false;
  }
  return true;
}

const string& MysqlConn::get_pool_name() { return m_dbpool->get_pool_name(); }

ResultSet MysqlConn::do_query(const string& sql) {
  mysql_ping(m_mysql);  // 若断开自动重连
  if (mysql_real_query(m_mysql, sql.c_str(), sizeof(sql))) {
    std::cout << "[ERR] mysql_real_query failed:" << mysql_error(m_mysql)
              << std::endl;
    return nullptr;
  }
  // 返回结果
  MYSQL_RES* res = mysql_store_result(m_mysql);
  if (!res) {
    std::cout << "[ERR] mysql_store_result failed:" << mysql_error(m_mysql)
              << std::endl;
    return nullptr;
  }
  ResultSet result_set(res);

  return result_set;
}

MysqlPool::MysqlPool(const string& pool_name, const string& db_server_ip,
                     uint16_t db_server_port, const string& username,
                     const string& password, const string& db_name,
                     uint32_t max_conn_cnt)
    : m_pool_name(pool_name),
      m_db_server_ip(db_server_ip),
      m_db_server_port(db_server_port),
      m_username(username),
      m_password(password),
      m_db_name(db_name),
      m_db_max_conn_cnt(max_conn_cnt) {}

MysqlPool::~MysqlPool() {
  std::lock_guard<std::mutex> lock(m_mutex); // 加锁
  m_abort_request = true;
  m_cond_var.notify_all();  // 通知所有在等待的请求退出

  for (list<MysqlConn*>::iterator it = m_free_list.begin();
       it != m_free_list.end(); it++) {
    MysqlConn* conn = *it;
    delete conn;
    conn = nullptr;
  }

  m_free_list.clear();
}

bool MysqlPool::init() {
  // 创建固定的最小连接数
  for (uint32_t i = 0; i < m_db_cur_conn_cnt; ++i) {
    MysqlConn* db_conn = new MysqlConn(this);
    if(!db_conn->init()) {
      delete db_conn;
      db_conn = nullptr;
      return false;
    }
    m_free_list.push_back(db_conn);
  }
  return true;
}

MysqlConn* MysqlPool::get_db_conn(const uint32_t timeout_ms = 0) {

}

void MysqlPool::release_db_conn(MysqlConn* pConn) {

}