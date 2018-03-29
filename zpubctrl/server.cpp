#include "server.hpp"
#include <stdexcept>
#include <queue>
#include <set>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <chrono>
#include <thread>
#include "server_ws.hpp"

using namespace std;
using chrono::milliseconds;

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
typedef WsServer::Connection Connection;
typedef std::shared_ptr<WsServer::Connection> ConnectionPtr;
typedef std::shared_ptr<WsServer::Message> MessagePtr;
typedef WsServer::SendStream SendStream;

namespace zpubctrl {

  struct Server::Detail {
    Detail(int port) :
      _server(),
      _pub_endpoint(_server.endpoint[default_pub_endpoint]),
      _ctrl_endpoint(_server.endpoint[default_ctrl_endpoint]) {

      _server.config.port = port;

      _ctrl_endpoint.on_message = [this](ConnectionPtr connection, MessagePtr message) {
        unique_lock<mutex> lock(_requests_mtx);
        _requests.push({connection, message});
        lock.unlock();
        _requests_cv.notify_one();
      };

      _pub_endpoint.on_open = [this](ConnectionPtr connection) {
        _subscribers.insert(connection);
      };

      _pub_endpoint.on_close = [this](ConnectionPtr connection, int status, const string& /*reason*/) {
        if (_subscribers.count(connection)) {
          _subscribers.erase(connection);
        }
      };
    }

    void start_thread() {
      _thread = thread([this]() {
        _server.start();
      });
    }

    void stop_thread() {
      _server.stop();
      _thread.join();
    }

    WsServer _server;
    WsServer::Endpoint& _pub_endpoint;
    WsServer::Endpoint& _ctrl_endpoint;
    std::mutex _requests_mtx;
    std::condition_variable _requests_cv;
    queue<pair<ConnectionPtr, MessagePtr>> _requests;
    set<ConnectionPtr> _subscribers;
    thread _thread;
  };

  Server::Server(int port) :
    _detail(new Detail(port)) {
    _detail->start_thread();
  }

  Server::~Server() { // Required for pimpl pattern
    _detail->stop_thread();
  }

  bool Server::wait_for_request(int timeout_ms, function<string(const string&)> request_handler) {

    unique_lock<mutex> lock(_detail->_requests_mtx);
    if (_detail->_requests_cv.wait_for(lock, milliseconds(timeout_ms), [&]() { return !_detail->_requests.empty(); })) {
      auto connection = _detail->_requests.front().first;
      auto message = _detail->_requests.front().second;
      _detail->_requests.pop();
      lock.unlock();

      string reply_payload;
      if (request_handler) {
        reply_payload = request_handler(message->string());
      }
      auto send_stream = make_shared<SendStream>();
      *send_stream << reply_payload;
      connection->send(send_stream, [&](const SimpleWeb::error_code& ec) {}, 130); // TODO: handle error
      return true;
    }

    return false;
  }

  void Server::publish_data(const string& payload) {
    auto send_stream = make_shared<SendStream>();
    *send_stream << payload;
    for (auto& subscriber : _detail->_subscribers) {
      subscriber->send(send_stream, [&](const SimpleWeb::error_code& ec) {}, 130); // TODO: handle error
    }
  }

}


