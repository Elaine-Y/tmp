#include "query_order.h"

#define ERROR_MSG_READ_DATA_FAIL "error when read message from proto stream"
#define ERROR_MSG_EMPTY_NAME "can't request an empty name"

Status HelloImpl::GetKey(::grpc::ServerContext *context,
                         const ::helloworld::Request *request,
                         ::helloworld::Response *response) {
  std::cout << "[INFO] server: receive from client." << std::endl;
  const Request &req = (*request);
  Status ret = Status::OK;
  do {
    if (!request) {
      return Status(grpc::INVALID_ARGUMENT, "INVALID_REQUEST");
    }
    if (req.out_trade_no().empty()) {
      return Status(grpc::INVALID_ARGUMENT, "PARAM_ERROR");
    }
    if (req.mchid().empty()) {
      return Status(grpc::INVALID_ARGUMENT, "PARAM_ERROR");
    }

    std::cout << "[DEBUG] req:" << req.ShortDebugString() << std::endl;
    deal_msg(req, response);
  } while (false);
  std::cout << "server: done for client" << std::endl;
  return ret;
}

Status HelloImpl::deal_msg(const Request &req, Response *rsp) {
  if (!m_db->init()) {
    std::cout << "[ERR] mysql pool init failed." << std::endl;
  }
  MysqlConn *db_conn = m_db->get_db_conn(2000);
  if (nullptr == db_conn) {
    std::cout << "[ERR] null db handler" << std::endl;
    rsp_err(*rsp, 1, std::string("get db conn failed"));
    return Status(grpc::UNKNOWN, "SYSTEMERROR");
  }
  // static const SqlConcat::Split stmt(
  //     "select appid,mchid,out_trade_no,transaction_id,"
  //     "trade_type,trade_state,trade_state_desc,"
  //     "bank_type,attach,success_time,open_id,total,"
  //     "payer_total,currency,payer_currency from business_order where mchid=?
  //     " "and out_trade_no=?");
  // std::ostringstream ss;
  // SqlConcat(ss, stmt) << req.mchid() << req.out_trade_no();
  // const std::string &sql = ss.str();
  std::string sql_t =
      "select appid from business_order where mchid=" + req.mchid() +
      " and "
      "out_trade_no=" +
      req.out_trade_no();

  ResultSet *res = db_conn->do_query(sql_t);
  // 数据唯一
  std::string appid;
  while (res->next()) {
    appid = res->get_string("appid");
    std::cout << "[Test] get appid from DB:" << appid << std::endl;
    rsp->set_appid(appid);
  }
  return Status(grpc::OK, "");
}