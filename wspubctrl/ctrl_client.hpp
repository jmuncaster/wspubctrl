#pragma once

#include "constants.hpp"
#include <functional>
#include <memory>
#include <string>

namespace wspubctrl {

  // Client issues synchronous control requests.
  class CtrlClient {
    public:
      CtrlClient(const std::string& ctrl_uri);
      ~CtrlClient();

      // Start client thread and connects to server.
      // @returns if connection succeeded
      // @throws on socket error or timeout
      void connect();

      // Stop client thread.
      void disconnect();

      // Synchronously issue a request and wait for the reply.
      // @returns contents of reply
      // @throws on socket error or timeout
      std::string request(const std::string& payload, int timeout_ms = default_request_timeout_ms);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };

}

