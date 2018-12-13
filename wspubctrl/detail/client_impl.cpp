#include "client_impl.hpp"
#include "../constants.hpp"
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std;

namespace wspubctrl {

  namespace detail {

    constexpr int connect_timeout_ms = 1000;
    constexpr int stop_thread_close_timeout_ms = 1000;

    ClientImpl::ClientImpl(const string& uri) :
      _client(uri),
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

    ClientImpl::~ClientImpl() {
      if (_state != State::disconnected) {
        stop_thread();
      }
    }

    void ClientImpl::connect() {
      unique_lock<mutex> lock(_mtx);
      start_thread();
      auto timeout_ms = connect_timeout_ms;
      if (!_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [&]() { return _state == State::connected; })) {
        throw runtime_error("timeout: could not establish first connection");
      }
    }

    void ClientImpl::disconnect() {
      stop_thread();
    }

    void ClientImpl::start_thread() {
      _thread = thread([this]() {
        _state = State::temporarily_disconnected;
        while (!_shutdown) {
          _client.start();
          if (!_shutdown) {
            this_thread::sleep_for(chrono::milliseconds(1000));
          }
        }
      });
    }

    void ClientImpl::stop_thread() {
      unique_lock<mutex> lock(_mtx);
      _shutdown = true;

      auto timeout_ms = stop_thread_close_timeout_ms;
      if (_connection && _state == State::connected) {
        _connection->send_close(0);
        _cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [&]() { return _state != State::connected;});
      }

      _client.stop();
      _thread.join();
      _state = State::disconnected;
    }

    string ClientImpl::request_and_wait_for_reply(const std::string& payload, int timeout_ms) {
      unique_lock<mutex> lock(_mtx);
      if (!_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [&]() { return _state == State::connected;})) {
        throw runtime_error("timeout: could not establish connection for request");
      }

      auto send_stream = make_shared<SendStream>();
      *send_stream << payload;
      _connection->send(send_stream);

      if (timeout_ms == forever) {
        _cnd.wait(lock);
      }
      else if (_cnd.wait_for(lock, chrono::milliseconds(timeout_ms)) == cv_status::timeout) {
        _connection->send_close(1);
        throw runtime_error("timeout: did not receive reply");
      }

      if (_state == State::connected) {
        return _payload;
      }
      else {
        throw runtime_error("connection error");
      }
    }

    string ClientImpl::poll(int timeout_ms) {
      string data;
      if (poll(data, timeout_ms)) {
        return data;
      }
      else {
        throw runtime_error("timeout");
      }
    }

    bool ClientImpl::poll(std::string& data, int timeout_ms) {
      unique_lock<mutex> lock(_mtx);
      if (timeout_ms == forever) {
        _cnd.wait(lock, [this]() { return _new_message; });
        _new_message = false;
        data = _payload;
        return true;
      }
      else if (_cnd.wait_for(lock, chrono::milliseconds(timeout_ms), [this]() { return _new_message; })) {
        _new_message = false;
        data = _payload;
        return true;
      }
      else {
        return false;
      }
    }

  } // detail
} // wspubctrl

