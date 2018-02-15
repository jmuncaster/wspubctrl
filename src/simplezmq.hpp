#pragma once

#include <zmq.hpp>

#include <memory>

namespace simplezmq {
  class Server {
    public:
      Server();
      bool get_request(std::string& payload);
      void reply(const std::string& payload);
      //void publish_data(const std::string& data);

    private:
      zmq::context_t _context;
      zmq::socket_t _reply_socket;
  };

  class Client {
    public:
      Client();
      std::string request(const std::string& payload);
      //bool check_for_data(std::string& data);

    private:
      zmq::context_t _context;
      zmq::socket_t _request_socket;
  };
}
