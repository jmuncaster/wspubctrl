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
typedef std::shared_ptr<WsServer::InMessage> InMessagePtr;

namespace wspubctrl {

  struct Server::Detail {
    Detail(const string& address, int port, const string& ctrl_endpoint_path) {
      _server.config.address = address;
      _server.config.port = port;
      setup_ctrl_endpoint(ctrl_endpoint_path);
    }

    void setup_ctrl_endpoint(const std::string& path) {
      auto& ctrl_endpoint = _server.endpoint[path];

      //ctrl_endpoint.on_open = [this](ConnectionPtr connection) {
      //  cout << "new connection to ctrl endpoint" << endl;
      //};

      ctrl_endpoint.on_message = [this](ConnectionPtr connection, InMessagePtr message) {
        unique_lock<mutex> lock(_requests_mtx);
        _requests.push({connection, message});
        lock.unlock();
        _requests_cv.notify_one();
      };
    }

    void add_publish_endpoint(const std::string& path) {
      auto& pub_endpoint = _server.endpoint[path];
      _subscribers[path] = {};

      pub_endpoint.on_open = [this, path](ConnectionPtr connection) {
        auto& subscribers = _subscribers[path];
        subscribers.insert(connection);
        //cout << "added subscriber to " << path << ": " << subscribers.size() << endl;
      };

      pub_endpoint.on_close = [this, path](ConnectionPtr connection, int status, const string& /*reason*/) {
        auto& subscribers = _subscribers[path];
        if (subscribers.count(connection)) {
          subscribers.erase(connection);
          //cout << "removed subscriber from " << path << ": " << subscribers.size() << endl;
        }
      };

      // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
      pub_endpoint.on_error = [this, path](ConnectionPtr connection, const SimpleWeb::error_code &ec) {
        auto& subscribers = _subscribers[path];
        if (subscribers.count(connection)) {
          subscribers.erase(connection);
          //cout << "removed subscriber from " << path << ": " << subscribers.size() << endl;
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
      if (_thread.joinable()) {
        _thread.join();
      }
    }

    WsServer _server;
    std::map<std::string, set<ConnectionPtr>> _subscribers;
    std::mutex _requests_mtx;
    std::condition_variable _requests_cv;
    queue<pair<ConnectionPtr, InMessagePtr>> _requests;
    thread _thread;
  };

  Server::Server(const string& address, int port, const string& ctrl_endpoint_path) :
    _detail(new Detail(address, port, ctrl_endpoint_path)) {
  }

  Server::Server(int port, const string& ctrl_endpoint_path) :
    Server(default_address, port, ctrl_endpoint_path) {
  }

  Server::~Server() { // Required for pimpl pattern
    _detail->stop_thread();
  }

  void Server::start() {
    _detail->start_thread();
  }

  void Server::add_publish_endpoint(const std::string& path) {
    _detail->add_publish_endpoint(path);
  }

  bool Server::handle_request(int timeout_ms, function<string(const string&)> request_handler) {

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
      connection->send(reply_payload, [&](const SimpleWeb::error_code& ec) {}); // TODO: handle error
      return true;
    }

    return false;
  }

  void Server::send(const string& endpoint_path, const string& payload) {
    auto& subscribers = _detail->_subscribers[endpoint_path];
    for (auto& subscriber : subscribers) {
      subscriber->send(payload, [&](const SimpleWeb::error_code& ec) {}); // TODO: handle error
    }
  }

}


