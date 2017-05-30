//
//  RLServerClient.cpp
//  RLServerClient
//
//  Created by anton on 29/04/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "RLServerClient.hpp"
#include <iostream>
//#include <functional>

RLServerClient::RLServerClient () : sendSync(1) {}

void RLServerClient::connect () {

  h.onConnection([this](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
    this->ws = ws;
    sendSync.reportProducerDone(0);
  });

  h.onMessage([this](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode opCode) {
    std::string dataStr (message, length);
    received.swap(dataStr);
    sendSync.reportProducerDone(0);
  });

  h.connect("ws://127.0.0.1:8765", nullptr);
  mainThread = std::thread ([this](){
    h.run();
  });
  sendSync.waitProducers();
  sendSync.reportConsumerDone();
}

void RLServerClient::disconnect () {
  h.getDefaultGroup<uWS::CLIENT>().close();
  mainThread.join();
}

void RLServerClient::send (
  const std::string& data,
  std::string& result
) {
  std::lock_guard<std::mutex> lock (mtxSend);
  ws->send(data.c_str(), data.length(), uWS::OpCode::TEXT);
  sendSync.waitProducers();
  result.swap(received);
  sendSync.reportConsumerDone();
}
