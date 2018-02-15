#pragma once

#include <functional>
#include <memory>

namespace simplezmq {

  constexpr int default_data_port = 5554;
  constexpr int default_ctrl_port = 5555;

  class Server {
    public:
      Server(int pub_port = default_data_port, int ctrl_port = default_ctrl_port);
      ~Server();
      bool check_for_request(std::function<std::string(const std::string&)> handler);
      void publish(const std::string& payload);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };

  class Client {
    public:
      Client(const std::string& server_address = "localhost", int sub_port = default_data_port, int ctrl_port = default_ctrl_port);
      ~Client();
      std::string request(const std::string& payload);
      std::string wait_for_data(int timeout_ms = -1);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };
}
