#include "server.hpp"
#include <stdexcept>
#include <queue>
#include <set>
#include <condition_variable>
#include <mutex>
#include <vector>
#include "server_ws.hpp"

using namespace std;

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
typedef WsServer::Connection Connection;
typedef std::shared_ptr<WsServer::Connection> ConnectionPtr;
typedef std::shared_ptr<WsServer::Message> MessagePtr;
typedef WsServer::SendStream SendStream;

namespace zpubctrl {

  struct Server::Detail {
    Detail() :
      _server(),
      _pub_endpoint(_server.endpoint["/pub"]),
      _ctrl_endpoint(_server.endpoint["/ctrl"]) {

      _server.config.port = 8080;

      _ctrl_endpoint.on_message = [this](ConnectionPtr connection, MessagePtr message) {
        _requests.push({connection, message});
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

    WsServer _server;
    WsServer::Endpoint& _pub_endpoint;
    WsServer::Endpoint& _ctrl_endpoint;
    queue<pair<ConnectionPtr, MessagePtr>> _requests;
    set<ConnectionPtr> _subscribers;
  };

  Server::Server(int pub_port, int ctrl_port) :
    _detail(new Detail) {
  }

  Server::~Server() { // Required for pimpl pattern
  }

  void Server::start() {
    _detail->_server.start();
  }

  bool Server::wait_for_request(int timeout_ms, function<string(const string&)> request_handler) {

    if (!_detail->_requests.empty()) {
      auto connection = _detail->_requests.front().first;
      auto message = _detail->_requests.front().second;
      _detail->_requests.pop();
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


