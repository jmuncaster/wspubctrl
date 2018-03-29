#pragma once

#include "constants.hpp"
#include <memory>
#include <string>

namespace zpubctrl {

  // Client subscribes to a data stream
  class SubClient {
    public:
      SubClient(const std::string& pub_uri = default_pub_uri);
      ~SubClient();

      void start();

      // Polls subscription for data
      // @param timeout_ms: Wait for this long for data. -1 means wait forever.
      // @returns received data
      // @throws on socket error or timeout
      std::string wait_for_data(int timeout_ms = forever);

    private:
      struct Detail;
      std::unique_ptr<Detail> _detail;
  };

}

