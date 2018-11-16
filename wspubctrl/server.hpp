#pragma once

#include "constants.hpp"
#include <functional>
#include <memory>
#include <string>

namespace wspubctrl {

  // Server publishes data stream and accepts synchronous control requests.
  class Server {
    public:
      Server(int port = default_port, const std::string& ctrl_endpoint = default_ctrl_endpoint);
      ~Server();

      // Start accepting connections
      void start();

      // Polls ctrl socket for request. If there is a request, respond with reply returned by request_handler.
      // @param request_handler: Callback that is called if and only if there is a request.
      // @returns true if a request was handled.
      typedef std::function<std::string(const std::string&)> request_callback_t;
      bool handle_request(int timeout_ms = forever, request_callback_t request_handler = nullptr);

      // Add a publish socket (e.g., "/pub")
      void add_publish_endpoint(const std::string& endpoint);

      // Publish a message to a pub endpoint
      void send(const std::string& endpoint, const std::string& payload);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };
}

