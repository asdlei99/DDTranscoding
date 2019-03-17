//
//  EvHttpHandler.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/6.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef EvHttpHandler_hpp
#define EvHttpHandler_hpp

#include <stdio.h>

extern "C" {
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/http.h>
};

class EvHttpHandler {
private:
public:
    static void handler404(struct evhttp_request *req, void *arg);
    static void add(struct evhttp_request *req, void *arg);
    static void del(struct evhttp_request *req, void *arg);
    static void list(struct evhttp_request *req, void *arg);
};

#endif /* EvHttpHandler_hpp */
