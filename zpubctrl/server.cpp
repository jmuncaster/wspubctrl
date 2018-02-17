#include "server.hpp"
#include <stdexcept>
#include <vector>
#include <zmq.hpp>

using namespace std;

namespace zpubctrl {

  struct Server::Detail {
    Detail() :
      _context(),
      _pub_socket(_context, ZMQ_PUB),
      _reply_socket(_context, ZMQ_REP) {
    }
    zmq::context_t _context;
    zmq::socket_t _pub_socket;
    zmq::socket_t _reply_socket;
  };

  Server::Server(int pub_port, int ctrl_port) :
    _detail(new Detail) {

    _detail->_pub_socket.setsockopt(ZMQ_LINGER, 0); // On shutdown, don't wait for queued outbound messages
    _detail->_pub_socket.setsockopt(ZMQ_SNDHWM, 1);

    _detail->_pub_socket.bind("tcp://*:" + to_string(pub_port));
    _detail->_reply_socket.bind("tcp://*:" + to_string(ctrl_port));
  }

  Server::~Server() { // Required for pimpl pattern
    _detail->_pub_socket.close();
    _detail->_reply_socket.close();
  }

  bool Server::wait_for_request(int timeout_ms, function<string(const string&)> request_handler) {
    vector<zmq::pollitem_t> items {{(void*)_detail->_reply_socket, 0, ZMQ_POLLIN, 0}};
    if (zmq::poll(items, timeout_ms)) {
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t msg;
        _detail->_reply_socket.recv(&msg);
        string request_payload(static_cast<char*>(msg.data()), msg.size());
        string reply_payload;
        if (request_handler) {
          reply_payload = request_handler(request_payload);
        }
        _detail->_reply_socket.send(reply_payload.data(), reply_payload.size());
        return true;
      }
    }

    return false;
  }

  void Server::publish_data(const string& payload) {
    _detail->_pub_socket.send(payload.data(), payload.size());
  }

}


