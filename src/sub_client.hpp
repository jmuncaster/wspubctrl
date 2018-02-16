#pragma once

#include "constants.hpp"
#include <memory>
#include <string>

namespace zpubctrl {

  // Client subscribes to a data stream
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

}

