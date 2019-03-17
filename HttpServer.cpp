//
//  HttpServer.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/5.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include "HttpServer.hpp"
#include "BaseException.hpp"
#include "EvHttpHandler.hpp"

void HttpServer::init() {
    base = event_base_new();
    httpServer = evhttp_new(base);
    if(httpServer == NULL) {
        throw new BaseException(HTTP_SERVER_CREATE_ERROR, "init evhttp_base error");
    }
    int ret = evhttp_bind_socket(httpServer, bindAddr.c_str(), port);
    if(ret != 0)
    {
        throw new BaseException(HTTP_SERVER_CREATE_ERROR, "bind socket error evhttp_bind_socket");
    }
    
    evhttp_set_gencb(httpServer, EvHttpHandler::handler404, NULL);
    evhttp_set_cb(httpServer, "/add", EvHttpHandler::add, workersContainer);
    evhttp_set_cb(httpServer, "/list", EvHttpHandler::list, workersContainer);
    evhttp_set_cb(httpServer, "/del", EvHttpHandler::del, workersContainer);
    cout<<"DDTranscoding server init complete !"<<endl;
}

void HttpServer::dispatch() {
     event_base_dispatch(base);
}
