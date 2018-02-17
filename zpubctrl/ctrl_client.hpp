#pragma once

#include "constants.hpp"
#include <functional>
#include <memory>
#include <string>

namespace zpubctrl {

  // Client issues synchronous control requests.
  class CtrlClient {
    public:
      CtrlClient(const std::string& server_address = default_host, int ctrl_port = default_ctrl_port);
      ~CtrlClient();

      // Synchronously issue a request and wait for the reply.
      // @returns contents of reply
      // @throws on socket error or timeout
      std::string request(const std::string& payload, int timeout_ms = default_request_timeout_ms);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };

}

