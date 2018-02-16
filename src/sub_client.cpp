#include "sub_client.hpp"
#include <stdexcept>
#include <vector>
#include <zmq.hpp>

using namespace std;

namespace zpubctrl {

  struct SubClient::Detail {
    Detail() :
      _context(),
      _sub_socket(_context, ZMQ_SUB) {
    }

    zmq::context_t _context;
    zmq::socket_t _sub_socket;
  };

  SubClient::SubClient(const string& server_address, int sub_port) :
    _detail(new Detail) {
    _detail->_sub_socket.setsockopt(ZMQ_SUBSCRIBE, "", 0); // Accept everything
    _detail->_sub_socket.setsockopt(ZMQ_CONFLATE, 1);      // Drop old messages
    _detail->_sub_socket.connect("tcp://" + server_address + ":" + to_string(sub_port));
  }

  SubClient::~SubClient() { // Required for pimpl pattern
    _detail->_sub_socket.close();
  }

  string SubClient::wait_for_data(int timeout_ms) {
    vector<zmq::pollitem_t> items {{(void*)_detail->_sub_socket, 0, ZMQ_POLLIN, 0}};
    if (zmq::poll(items, timeout_ms)) {
      if (items[0].revents & ZMQ_POLLIN) {
        zmq::message_t msg;
        _detail->_sub_socket.recv(&msg);
        string payload(static_cast<char*>(msg.data()), msg.size());
        return payload;
      }
    }
    return {};
  }

}

