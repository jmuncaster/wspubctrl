#include "sub_client.hpp"
#include <stdexcept>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "client_ws.hpp"

using namespace std;

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
typedef WsClient::Connection Connection;
typedef std::shared_ptr<WsClient::Connection> ConnectionPtr;
typedef std::shared_ptr<WsClient::Message> MessagePtr;
typedef WsClient::SendStream SendStream;

namespace zpubctrl {

  struct SubClient::Detail {
    Detail() :
      _client("localhost:8080/pub"),
      _mutex(),
      _new_message(false),
      _payload() {
      _client.on_message = [this](ConnectionPtr connection, MessagePtr message) {
        unique_lock<mutex> lock(_mutex);
        _payload = message->string();
        _new_message = true;
        lock.unlock();
        _cnd.notify_one();
      };
    }

    WsClient _client;
    mutex _mutex;
    bool _new_message;
    string _payload;
    condition_variable _cnd;
  };

  SubClient::SubClient(const string& server_address, int sub_port) :
    _detail(new Detail) {
  }

  SubClient::~SubClient() { // Required for pimpl pattern
  }

  void SubClient::start() {
    _detail->_client.start();
  }

  string SubClient::wait_for_data(int timeout_ms) {
    unique_lock<mutex> lock(_detail->_mutex);
    if (_detail->_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [this]() { return _detail->_new_message; })) {
      _detail->_new_message = false;
      return _detail->_payload;
    };
    throw runtime_error("timeout");
  }

}

