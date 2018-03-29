#include "ctrl_client.hpp"
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

  namespace {
    enum class State { disconnected, connected, error};
  }

  struct CtrlClient::Detail {

    Detail(const string& ctrl_uri) :
      _client(ctrl_uri),
      _mtx(),
      _new_message(false),
      _payload() {

      _client.on_open = [this](ConnectionPtr connection) {
        unique_lock<mutex> lock(_mtx);
        _connection = connection;
        _state = State::connected;
        lock.unlock();
        _cnd.notify_one();
      };

      _client.on_message = [this](ConnectionPtr connection, MessagePtr message) {
        unique_lock<mutex> lock(_mtx);
        _payload = message->string();
        _state = State::connected;
        lock.unlock();
        _cnd.notify_one();
      };

      _client.on_error = [this](ConnectionPtr connection, const SimpleWeb::error_code &ec) {
        unique_lock<mutex> lock(_mtx);
        _connection = {};
        _state = State::error;
        lock.unlock();
        _cnd.notify_one();
      };

      _client.on_close = [this](ConnectionPtr connection, int status, const string& reason) {
        unique_lock<mutex> lock(_mtx);
        _connection = {};
        _state = State::disconnected;
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
    ConnectionPtr _connection;
    mutex _mtx;
    bool _new_message;
    bool _error;
    string _payload;
    condition_variable _cnd;
    thread _thread;
    State _state = State::disconnected;
    bool _reconnect = true;
  };

  CtrlClient::CtrlClient(const string& ctrl_uri) :
    _detail(new Detail(ctrl_uri)) {
    _detail->start_thread();
  }

  CtrlClient::~CtrlClient() { // Required for pimpl pattern
    _detail->stop_thread();
  }

  string CtrlClient::request(const string& payload, int timeout_ms) {
    unique_lock<mutex> lock(_detail->_mtx);
    if (!_detail->_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [&]() { return _detail->_state == State::connected;})) {
      throw runtime_error("timeout: could not establish connection");
    }

    auto send_stream = make_shared<SendStream>();
    *send_stream << payload;
    _detail->_connection->send(send_stream);

    if (timeout_ms == forever) {
      _detail->_cnd.wait(lock);
    }
    else if (_detail->_cnd.wait_for(lock, chrono::milliseconds(timeout_ms)) == cv_status::timeout) {
      _detail->_connection->send_close(1);
      throw runtime_error("timeout: did not receive reply");
    }

    if (_detail->_state == State::connected) {
      return _detail->_payload;
    }
    else {
      throw runtime_error("connection error");
    }
  }
}

