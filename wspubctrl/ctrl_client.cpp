#include "ctrl_client.hpp"
#include "detail/client_impl.hpp"

using namespace std;

namespace wspubctrl {

  CtrlClient::CtrlClient(const string& ctrl_uri) :
    _impl(new detail::ClientImpl(ctrl_uri)) {
  }

  CtrlClient::~CtrlClient() { // Required for pimpl pattern
  }

  void CtrlClient::connect() {
    _impl->connect();
  }

  void CtrlClient::disconnect() {
    _impl->disconnect();
  }

  string CtrlClient::request(const string& payload, int timeout_ms) {
    return _impl->request_and_wait_for_reply(payload, timeout_ms);
  }
}

