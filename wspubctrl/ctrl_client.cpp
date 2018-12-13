#include "ctrl_client.hpp"
#include "detail/client_detail.hpp"
#include "constants.hpp"
#include <condition_variable>
#include <mutex>
#include <stdexcept>

using namespace std;

namespace wspubctrl {

  CtrlClient::CtrlClient(const string& ctrl_uri) :
    _detail(new detail::ClientDetail(ctrl_uri)) {
  }

  CtrlClient::~CtrlClient() { // Required for pimpl pattern
  }

  void CtrlClient::connect() {
    _detail->connect();
  }

  void CtrlClient::disconnect() {
    _detail->disconnect();
  }

  string CtrlClient::request(const string& payload, int timeout_ms) {
    return _detail->request_and_wait_for_reply(payload, timeout_ms);
  }
}

