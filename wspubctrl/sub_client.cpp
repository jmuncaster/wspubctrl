#include "sub_client.hpp"
#include "detail/client_detail.hpp"
#include "constants.hpp"
#include <mutex>
#include <stdexcept>

using namespace std;

namespace wspubctrl {

  SubClient::SubClient(const string& pub_uri) :
    _detail(new detail::ClientDetail(pub_uri)) {
  }

  SubClient::~SubClient() { // Required for pimpl pattern
  }

  void SubClient::connect() {
    _detail->connect();
  }

  void SubClient::disconnect() {
    _detail->disconnect();
  }

  string SubClient::poll(int timeout_ms) {
    return _detail->poll(timeout_ms);
  }

  bool SubClient::poll(std::string& data, int timeout_ms) {
    return _detail->poll(data, timeout_ms);
  }
}

