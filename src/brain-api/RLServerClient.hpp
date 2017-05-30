//
//  RLServerClient.hpp
//  RLServerClient
//
//  Created by anton on 27/05/17.
//  Copyright © 2017 anton. All rights reserved.
//

#ifndef RLServerClient_hpp
#define RLServerClient_hpp

#include "uWS.h"
#include "ConsumerProducerSyncronizer.hpp"

class RLServerClient {
public:

  RLServerClient ();

  void connect ();
  void disconnect ();
  void send (
    const std::string& data,
    std::string& result
  );

private:

  std::thread mainThread;
  std::mutex mtxSend;
  uWS::Hub h;
  uWS::WebSocket<uWS::CLIENT> *ws;
  std::string received;
  ConsumerProducerSyncronizer sendSync;

};

#endif /* RLServerClient_hpp */
