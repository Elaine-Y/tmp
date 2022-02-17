// internal stl
#include <unistd.h>

#include <iostream>
#include <random>
#include <string>

// lib outside
// #include <fmt/core.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "sqlite3.h"

// user lib
#include "proto/hello.grpc.pb.h"
#include "utils/utils.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using helloworld::Hello;
using helloworld::Request;
using helloworld::Response;

#define ERROR_MSG_READ_DATA_FAIL "error when read message from proto stream"
#define ERROR_MSG_EMPTY_NAME "can't request an empty name"

std::mt19937 myrand(time(nullptr));
;

class HelloImpl final : public Hello::Service {
 public:
  explicit HelloImpl(sqlite3 *db) : db(db) {}
  Status GetKey(::grpc::ServerContext *context,
                const ::helloworld::Request *request,
                ::helloworld::Response *response) override {
    // fmt::print("server: receive from client.\n");
    std::cout << "server: receive from client." << std::endl;
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

      // fmt::print("server: get out_trade_no \"{}\"\n",
      //            req.out_trade_no().c_str());
      // fmt::print("server: get mchid \"{}\"\n", req.mchid().c_str());
      // fmt::print("server: start write message \"hello {}\" \n",
      //            req.out_trade_no().c_str());
      Response rsp;
      // rsp.set_appid(fmt::format("hello {}:{}, your is: {}", req.out_trade_no(),
      //                           req.mchid(),
      //                           deal_with_message(req.out_trade_no())));
      rsp.set_appid("hello {" + req.out_trade_no() + "}:{" + req.mchid() +
                    "}, your is:{" + deal_with_message(req.out_trade_no()) +
                    "}");

      // fmt::print("server: success write message \"hello {}\"  \n",
      //            req.out_trade_no().c_str());
    } while (false);
    // fmt::print("server: done for client.\n");
    std::cout << "server: done for client" << std::endl;
    return res;
  }

 private:
  sqlite3 *db;

  std::string deal_with_message(const std::string &name) {
    // fmt::print("server: deal with message now.\n");
    std::cout << "server: deal with message now" << std::endl;
    std::string val;
    if (utils::GetDB(db, name, &val) != SQLITE_OK) {
      for (int i = 0; i < 10; ++i) {
        val += (myrand() % 10) + '0';
      }
      // fmt::print("server: can't search {} in db, create a new one now {}.\n",
      //            name, val);
      if (utils::PutDB(db, name, val) == SQLITE_OK) {
        // fmt::print("server: save {}:{} success.\n", name, val);
      }
    }
    // fmt::print("server: down with message now. {}:{}\n", name, val);
    std::cout << "server: down with message now." << name << ":" << val << std::endl;
    return val;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50001");

  // init a impl class
  sqlite3 *db;
  utils::CreateDB("./data.db", &db);
  HelloImpl service(db);

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();

  // init a builder
  ServerBuilder builder;
  // set builder address and credential
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // register service to build
  builder.RegisterService(&service);

  // start a truly server by builder
  std::unique_ptr<Server> server(builder.BuildAndStart());
  // fmt::print("Server listening on {}\n", server_address);
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

void print_wrk_dir() {
  char buffer[250];
  getcwd(buffer, 250);
  // fmt::print("The current directory is: {}\n", buffer);
}

int main(int argc, char *argv[]) {
  print_wrk_dir();
  RunServer();
  return 0;
}
