#include "sub_client.hpp"
#include "detail/client_impl.hpp"

using namespace std;

namespace wspubctrl {

  SubClient::SubClient(const string& pub_uri) :
    _impl(new detail::ClientImpl(pub_uri)) {
  }

  SubClient::~SubClient() { // Required for pimpl pattern
  }

  void SubClient::connect() {
    _impl->connect();
  }

  void SubClient::disconnect() {
    _impl->disconnect();
  }

  string SubClient::poll(int timeout_ms) {
    return _impl->poll(timeout_ms);
  }

  bool SubClient::poll(std::string& data, int timeout_ms) {
    return _impl->poll(data, timeout_ms);
  }
}

