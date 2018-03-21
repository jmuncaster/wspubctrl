#include "ctrl_client.hpp"
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

  struct CtrlClient::Detail {
    Detail() :
      _client("localhost:8080/ctrl"),
      _mutex(),
      _new_message(false),
      _payload() {

      _client.on_open = [this](ConnectionPtr connection) {
        unique_lock<mutex> lock(_mutex);
        _connection = connection;
        lock.unlock();
        _cnd.notify_one();
      };

      _client.on_message = [this](ConnectionPtr connection, MessagePtr message) {
        unique_lock<mutex> lock(_mutex);
        _payload = message->string();
        _new_message = true;
        lock.unlock();
        _cnd.notify_one();
      };
    }

    WsClient _client;
    ConnectionPtr _connection;
    mutex _mutex;
    bool _new_message;
    string _payload;
    condition_variable _cnd;
  };

  CtrlClient::CtrlClient(const string& server_address, int ctrl_port) :
    _detail(new Detail) {
  }

  CtrlClient::~CtrlClient() { // Required for pimpl pattern
  }

  void CtrlClient::start() {
    _detail->_client.start();
  }

  string CtrlClient::request(const string& payload, int timeout_ms) {
    if (!_detail->_connection) {
      unique_lock<mutex> lock(_detail->_mutex);
      _detail->_cnd.wait(lock, [&]() { return _detail->_connection; });
    }

    auto send_stream = make_shared<SendStream>();
    *send_stream << payload;
    _detail->_connection->send(send_stream);

    unique_lock<mutex> lock(_detail->_mutex);
    if (_detail->_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [this]() { return _detail->_new_message; })) {
      _detail->_new_message = false;
      return _detail->_payload;
    };
    throw runtime_error("timeout");
  }

}

