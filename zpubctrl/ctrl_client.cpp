#include "ctrl_client.hpp"
#include <stdexcept>
#include <vector>
#include <zmq.hpp>

using namespace std;

namespace zpubctrl {

  struct CtrlClient::Detail {
    Detail() :
      _context(),
      _request_socket(_context, ZMQ_REQ) {
    }
    zmq::context_t _context;
    zmq::socket_t _request_socket;
  };

  CtrlClient::CtrlClient(const string& server_address, int ctrl_port) :
    _detail(new Detail) {
    _detail->_request_socket.setsockopt(ZMQ_LINGER, 0); // On shutdown, don't wait for queued outbound messages
    _detail->_request_socket.connect("tcp://" + server_address + ":" + to_string(ctrl_port));
  }

  CtrlClient::~CtrlClient() { // Required for pimpl pattern
    _detail->_request_socket.close();
  }

  string CtrlClient::request(const string& payload, int timeout_ms) {
    _detail->_request_socket.send(payload.data(), payload.size());
    vector<zmq::pollitem_t> items {{(void*)_detail->_request_socket, 0, ZMQ_POLLIN, 0}};
    if (zmq::poll(items, timeout_ms)) {
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t msg;
        _detail->_request_socket.recv(&msg);
        return string(static_cast<char*>(msg.data()), msg.size());
      }
      else {
        throw runtime_error("socket error");
      }
    }
    else {
      throw runtime_error("timeout");
    }
  }

}

