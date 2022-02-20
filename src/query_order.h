#pragma once
#include <iostream>
#include <string>

#include "mysql/mysql_pool.h"
#include "proto/hello.grpc.pb.h"
using grpc::ServerContext;
using grpc::Status;
using helloworld::Hello;
using helloworld::Request;
using helloworld::Response;

class HelloImpl final : public Hello::Service {
 public:
  explicit HelloImpl(MysqlPool *db) : m_db(db) {}
  Status GetKey(::grpc::ServerContext *context,
                const ::helloworld::Request *request,
                ::helloworld::Response *response) override;

 private:
  MysqlPool *m_db;
  // void deal_msg(const Request &req, Response* rsp, Status *ret);
  Status deal_msg(const Request &req, Response* rsp);
  void rsp_err(Response& rsp, int err_code, const std::string& err_msg) {
    rsp.set_err_code(err_code);
    rsp.set_err_msg(err_msg);
  }
};