#include "query_order.h"

#define ERROR_MSG_READ_DATA_FAIL "error when read message from proto stream"
#define ERROR_MSG_EMPTY_NAME "can't request an empty name"

Status HelloImpl::GetKey(::grpc::ServerContext *context,
              const ::helloworld::Request *request,
              ::helloworld::Response *response) override {
  std::cout << "[INFO] server: receive from client." << std::endl;
  const Request &req = (*request);
  Status res = Status::OK;
  do {
    if (!request) {
      res = Status(grpc::ABORTED, ERROR_MSG_READ_DATA_FAIL);
      break;
    }
    if (req.out_trade_no().empty()) {
      res = Status(grpc::ABORTED, ERROR_MSG_EMPTY_NAME);
      break;
    }
    if (req.mchid().empty()) {
      res = Status(grpc::ABORTED, ERROR_MSG_EMPTY_NAME);
      break;
    }
    std::cout << "[DEBUG] req:" << req.ShortDebugString() << std::endl;

    Response rsp;
    deal_msg(req, &rsp);

  } while (false);
  std::cout << "server: done for client" << std::endl;
  return res;
}

void HelloImpl::deal_msg(const Request &req, Response* rsp) {
  MysqlConn *db_handler = m_db->get_db_conn(2000);
    if (nullptr==db_handler) {
      std::cout << "[ERR] null db handler" << std::endl;
      return;
    }
}