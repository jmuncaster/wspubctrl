#pragma once

#include "client_ws.hpp"
#include <condition_variable>
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace wspubctrl {
  namespace detail {
    using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
    typedef WsClient::Connection Connection;
    typedef std::shared_ptr<WsClient::Connection> ConnectionPtr;
    typedef std::shared_ptr<WsClient::Message> MessagePtr;
    typedef WsClient::SendStream SendStream;


    class ClientDetail{
      public:
        ClientDetail(const std::string& uri);
        ~ClientDetail();

        void connect();
        void disconnect();

        std::string request_and_wait_for_reply(const std::string& payload, int timeout_ms); // throws
        std::string poll(int timeout_ms); // throws
        bool poll(std::string& data, int timeout_ms); // does not throw

      private:
        void start_thread();
        void stop_thread();

        enum class State { disconnected, connected, temporarily_disconnected, error};

        WsClient _client;
        ConnectionPtr _connection;
        std::mutex _mtx;
        bool _new_message;
        bool _error;
        std::string _payload;
        std::condition_variable _cnd;
        std::thread _thread;
        State _state = State::disconnected;
        bool _shutdown = false;
    };
  }
}

