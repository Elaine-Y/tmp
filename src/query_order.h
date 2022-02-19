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
  void deal_msg(const Request &req, Response* rsp);
};