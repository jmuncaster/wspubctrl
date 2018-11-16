#include "sub_client.hpp"
#include <stdexcept>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "client_ws.hpp"

using namespace std;

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
typedef WsClient::Connection Connection;
typedef std::shared_ptr<WsClient::Connection> ConnectionPtr;
typedef std::shared_ptr<WsClient::Message> MessagePtr;
typedef WsClient::SendStream SendStream;

namespace wspubctrl {

  struct SubClient::Detail {
    Detail(const string& pub_uri)
    :  _client(pub_uri),
      _mtx(),
      _new_message(false),
      _payload() {
      _client.on_message = [this](ConnectionPtr connection, MessagePtr message) {
        unique_lock<mutex> lock(_mtx);
        _payload = message->string();
        _new_message = true;
        lock.unlock();
        _cnd.notify_one();
      };
    }

    void start_thread() {
      _thread = thread([this]() {
        for (;;) {
          _client.start();
          if (!_reconnect) {
            break;
          }
          this_thread::sleep_for(chrono::milliseconds(1000));
        }
      });
    }

    void stop_thread() {
      _reconnect = false;
      _client.stop();
      _thread.join();
    }

    WsClient _client;
    mutex _mtx;
    bool _new_message;
    string _payload;
    condition_variable _cnd;
    thread _thread;
    bool _reconnect = true;
  };

  SubClient::SubClient(const string& pub_uri) :
    _detail(new Detail(pub_uri)) {
  }

  SubClient::~SubClient() { // Required for pimpl pattern
    _detail->stop_thread();
  }

  void SubClient::start() {
    _detail->start_thread();
  }

  string SubClient::wait_for_data(int timeout_ms) {
    unique_lock<mutex> lock(_detail->_mtx);
    if (timeout_ms == forever) {
      _detail->_cnd.wait(lock, [this]() { return _detail->_new_message; });
      _detail->_new_message = false;
      return _detail->_payload;
    }
    else if (_detail->_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [this]() { return _detail->_new_message; })) {
      _detail->_new_message = false;
      return _detail->_payload;
    }
    else {
      throw runtime_error("timeout");
    }
  }
}

