// internal stl
#include <unistd.h>

// lib outside
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

// user lib
#include "query_order.h"
#include "utils/utils.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerWriter;

void RunServer() {
  string server_address("0.0.0.0:50001");
  // DB config
  const string &pool_name = "mysql_pool";
  const string &db_server_ip = "127.0.0.1";
  const uint16_t db_server_port = 3306;
  const string &username = "root";
  const string &password = "123456";
  const string &db_name = "demo";
  uint32_t max_conn_cnt = 2;  // todo
  MysqlPool db_pool(pool_name, db_server_ip, db_server_port, username, password,
                    db_name, max_conn_cnt);
  if (!db_pool.init()) {
    std::cout << "db init failed." << std::endl;
    return;
  }

  HelloImpl service(&db_pool);

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();

  // init a builder
  ServerBuilder builder;
  // set builder address and credential, todo secure
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // register service to build
  builder.RegisterService(&service);

  // start a truly server by builder
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "[INFO] Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char *argv[]) {
  RunServer();
  return 0;
}
