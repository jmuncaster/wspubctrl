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

  namespace {
    enum class State { disconnected, connected, temporarily_disconnected, error};
  }

  struct SubClient::Detail {
    Detail(const string& pub_uri)
    :  _client(pub_uri),
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
        _new_message = true;
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
        _state = State::temporarily_disconnected;
        lock.unlock();
        _cnd.notify_one();
      };
    }

    void start_thread() {
      _thread = thread([this]() {
        _state = State::temporarily_disconnected;
        while (!_shutdown) {
          cout << "_client.start())" << endl;
          _client.start();
          if (!_shutdown) {
            this_thread::sleep_for(chrono::milliseconds(1000));
          }
        }
      });

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
        _state = State::temporarily_disconnected;
        lock.unlock();
        _cnd.notify_one();
      };
    }

    void stop_thread() {
      unique_lock<mutex> lock(_mtx);
      _shutdown = true;

      auto timeout_ms = default_request_timeout_ms;
      if (_connection && _state == State::connected) {
        _connection->send_close(0);
        _cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [&]() { return _state != State::connected;});
      }

      _client.stop();
      _thread.join();
      _state = State::disconnected;
    }

    WsClient _client;
    ConnectionPtr _connection;
    mutex _mtx;
    bool _new_message;
    string _payload;
    condition_variable _cnd;
    thread _thread;
    State _state = State::disconnected;
    bool _shutdown = false;
  };

  SubClient::SubClient(const string& pub_uri) :
    _detail(new Detail(pub_uri)) {
  }

  SubClient::~SubClient() { // Required for pimpl pattern
    if (_detail->_state != State::disconnected) {
      disconnect();
    }
  }

  void SubClient::connect() {
    unique_lock<mutex> lock(_detail->_mtx);
    _detail->start_thread();
    auto timeout_ms = default_request_timeout_ms;
    if (!_detail->_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [&]() { return _detail->_state == State::connected;})) {
      throw runtime_error("timeout: could not establish first connection");
    }
  }

  void SubClient::disconnect() {
    _detail->stop_thread();
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

