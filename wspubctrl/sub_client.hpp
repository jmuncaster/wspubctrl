#pragma once

#include "constants.hpp"
#include <memory>
#include <string>

namespace wspubctrl {

  namespace detail {
    class ClientDetail;
  }

  // Client subscribes to a data stream
  class SubClient {
    public:
      SubClient(const std::string& pub_uri);
      ~SubClient();

      // Start client thread and connects to server.
      // @returns if connection succeeded
      // @throws on socket error or timeout
      void connect();

      // Attempts to close socket connection and waits short time. Stops client thread and returns regardless.
      // Does not throw.
      void disconnect();

      // Polls subscription for data
      // @param timeout_ms: Wait for this long for data. -1 means wait forever.
      // @returns received data
      // @throws on socket error or timeout
      std::string wait_for_data(int timeout_ms = forever);

    private:
      std::unique_ptr<detail::ClientDetail> _detail;
  };
} // wspubctrl

