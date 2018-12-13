#pragma once

#include "constants.hpp"
#include <memory>
#include <string>

namespace wspubctrl {

  namespace detail {
    class ClientDetail;
  }

  // Client issues synchronous control requests.
  class CtrlClient {
    public:
      CtrlClient(const std::string& ctrl_uri);
      ~CtrlClient();

      // Start client thread and connects to server.
      // @returns if connection succeeded
      // @throws on socket error or timeout
      void connect();

      // Attempts to close socket connection and waits short time. Stops client thread and returns regardless.
      // Does not throw.
      void disconnect();

      // Synchronously issue a request and wait for the reply.
      // @returns contents of reply
      // @throws on socket error or timeout
      std::string request(const std::string& payload, int timeout_ms = default_request_timeout_ms);

    private:
      std::unique_ptr<detail::ClientDetail> _detail;
  };
} // wspubctrl

