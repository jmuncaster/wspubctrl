#pragma once

#include <functional>
#include <memory>
#include <string>

namespace zpubctrl {

  constexpr int default_data_port = 5554;
  constexpr int default_ctrl_port = 5555;

  // Server publishes data stream and accepts synchronous control requests.
  class Server {
    public:
      Server(int pub_port = default_data_port, int ctrl_port = default_ctrl_port);
      ~Server();

      // Polls ctrl socket for request. If there is a request, respond with reply.
      // @param request_handler: Callback that is called if and only if there is a request.
      // @returns true if a request was handled.
      typedef std::function<std::string(const std::string&)> request_callback_t;
      bool wait_for_request(int timeout_ms = -1, request_callback_t request_handler = nullptr);

      // Publish a message to the pub socket
      void publish_data(const std::string& payload);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };

  // Client for subscribing to a data stream and synchronously issuing control requests.
  class SubClient {
    public:
      SubClient(const std::string& server_address = "localhost", int sub_port = default_data_port);
      ~SubClient();

      // Polls subscription for data
      // @param timeout_ms: Wait for this long for data. -1 means wait forever.
      // @returns received data, or empty string on timeout
      std::string wait_for_data(int timeout_ms = -1);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };

  // Client for subscribing to a data stream and synchronously issuing control requests.
  class CtrlClient {
    public:
      CtrlClient(const std::string& server_address = "localhost", int ctrl_port = default_ctrl_port);
      ~CtrlClient();

      // Synchronously issue a request and wait for the reply.
      // @returns contents of reply
      std::string request(const std::string& payload, int timeout_ms = 5000);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };
}

