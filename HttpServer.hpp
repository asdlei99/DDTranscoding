//
//  HttpServer.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/5.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef HttpServer_hpp
#define HttpServer_hpp

#include <stdio.h>
#include <iostream>
#include <string.h>
#include "WorkersContainer.hpp"
#include "WorkContainerProxy.hpp"

extern "C" {
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
};

using namespace std;

class HttpServer {
private:
    WorkersContainer* workersContainer;
    WorkContainerProxy* workersContainerProxy;
    unsigned short port;
    string bindAddr;
    struct event_base * base;
    struct evhttp * httpServer;
    void init();
public:
    HttpServer(string bindAddr, unsigned short port) {
        workersContainer = new WorkersContainer();
        workersContainerProxy = WorkContainerProxy::getWorkContainerProxy(workersContainer);
        this->port = port;
        this->bindAddr = bindAddr;
        init();
    }
    
    void dispatch();
    
    ~HttpServer() {
        delete workersContainer;
        evhttp_free(httpServer);
        event_base_free(base);
    }
};

#endif /* HttpServer_hpp */
