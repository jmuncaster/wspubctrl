#pragma once

#include <zmq.hpp>

#include <functional>
#include <memory>

namespace simplezmq {
  class Server {
    public:
      Server();
      bool check_for_request(std::function<std::string(const std::string&)> handler);
      void publish(const std::string& payload);

    private:
      zmq::context_t _context;
      zmq::socket_t _reply_socket;
      zmq::socket_t _pub_socket;
  };

  class Client {
    public:
      Client();
      std::string request(const std::string& payload);
      std::string wait_for_data(int timeout_ms = -1);

    private:
      zmq::context_t _context;
      zmq::socket_t _request_socket;
      zmq::socket_t _sub_socket;
  };
}
